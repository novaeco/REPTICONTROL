#include "power_manager.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_pm.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "pin_mapping.h"
#include "event_logger.h"

static const char *TAG = "power_manager";

// ADC handles
static adc_oneshot_unit_handle_t adc1_handle;
static adc_cali_handle_t adc1_cali_handle;

// Current state
static battery_state_t battery_state = BATTERY_STATE_NORMAL;
static power_mode_t current_mode = POWER_MODE_NORMAL;
static bool is_charging = false;
static int battery_level = 100;
static uint32_t battery_voltage = 4200;

// Battery thresholds (mV)
#define BATTERY_FULL_MV      4200
#define BATTERY_LOW_MV       3300
#define BATTERY_CRITICAL_MV  3000

// Initialize power management
esp_err_t power_manager_init(void) {
    ESP_LOGI(TAG, "Initializing power management");

    // Configure ADC for battery voltage monitoring
    adc_oneshot_unit_init_cfg_t adc_config = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc_config, &adc1_handle));

    // Configure ADC channel
    adc_oneshot_chan_cfg_t chan_config = {
        .bitwidth = ADC_BITWIDTH_12,
        .atten = ADC_ATTEN_DB_11,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, BAT_VOLTAGE_PIN, &chan_config));

    // Initialize ADC calibration
    adc_cali_handle_t handle = NULL;
    esp_err_t ret = adc_cali_create_scheme_curve_fitting(&handle);
    if (ret == ESP_OK) {
        adc1_cali_handle = handle;
    }

    // Configure battery status pins
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pin_bit_mask = (1ULL << BAT_CHARGE_PIN) | (1ULL << BAT_STDBY_PIN)
    };
    gpio_config(&io_conf);

    // Configure status LEDs
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << LED_POWER) | (1ULL << LED_STATUS) | (1ULL << LED_ERROR);
    gpio_config(&io_conf);

    // Turn on power LED
    gpio_set_level(LED_POWER, 1);

    // Configure power management
    esp_pm_config_esp32s3_t pm_config = {
        .max_freq_mhz = 240,
        .min_freq_mhz = 80,
        .light_sleep_enable = true
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));

    event_logger_add("Power management initialized", false);
    return ESP_OK;
}

// Get battery level
int power_manager_get_battery_level(void) {
    return battery_level;
}

// Get battery state
battery_state_t power_manager_get_battery_state(void) {
    return battery_state;
}

// Get battery voltage
uint32_t power_manager_get_battery_voltage(void) {
    return battery_voltage;
}

// Check charging status
bool power_manager_is_charging(void) {
    return is_charging;
}

// Set power mode
esp_err_t power_manager_set_mode(power_mode_t mode) {
    if (mode == current_mode) {
        return ESP_OK;
    }

    switch (mode) {
        case POWER_MODE_NORMAL:
            // Maximum performance
            esp_pm_config_esp32s3_t pm_config = {
                .max_freq_mhz = 240,
                .min_freq_mhz = 240,
                .light_sleep_enable = false
            };
            ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
            break;

        case POWER_MODE_ECO:
            // Balanced power saving
            esp_pm_config_esp32s3_t pm_config_eco = {
                .max_freq_mhz = 240,
                .min_freq_mhz = 80,
                .light_sleep_enable = true
            };
            ESP_ERROR_CHECK(esp_pm_configure(&pm_config_eco));
            break;

        case POWER_MODE_SLEEP:
            // Maximum power saving
            esp_pm_config_esp32s3_t pm_config_sleep = {
                .max_freq_mhz = 160,
                .min_freq_mhz = 80,
                .light_sleep_enable = true
            };
            ESP_ERROR_CHECK(esp_pm_configure(&pm_config_sleep));
            break;
    }

    current_mode = mode;
    event_logger_add_fmt("Power mode changed to %d", false, mode);
    return ESP_OK;
}

// Enter light sleep
esp_err_t power_manager_light_sleep(uint32_t sleep_ms) {
    if (is_charging || battery_state == BATTERY_STATE_CRITICAL) {
        return ESP_FAIL;
    }

    // Configure wake-up sources
    esp_sleep_enable_timer_wakeup(sleep_ms * 1000);
    esp_sleep_enable_gpio_wakeup();

    // Enter light sleep
    esp_light_sleep_start();

    return ESP_OK;
}

// Update power management status
void power_manager_update(void) {
    // Read battery voltage
    int raw_value;
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, BAT_VOLTAGE_PIN, &raw_value));

    int voltage_mv;
    if (adc1_cali_handle) {
        ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc1_cali_handle, raw_value, &voltage_mv));
    } else {
        voltage_mv = raw_value * 2; // Approximate conversion
    }

    battery_voltage = voltage_mv;

    // Calculate battery percentage
    if (voltage_mv >= BATTERY_FULL_MV) {
        battery_level = 100;
    } else if (voltage_mv <= BATTERY_CRITICAL_MV) {
        battery_level = 0;
    } else {
        battery_level = ((voltage_mv - BATTERY_CRITICAL_MV) * 100) /
                       (BATTERY_FULL_MV - BATTERY_CRITICAL_MV);
    }

    // Update charging status
    bool charging = !gpio_get_level(BAT_CHARGE_PIN);
    bool standby = !gpio_get_level(BAT_STDBY_PIN);

    is_charging = charging && !standby;

    // Update battery state
    battery_state_t new_state;

    if (is_charging) {
        new_state = standby ? BATTERY_STATE_FULL : BATTERY_STATE_CHARGING;
    } else if (voltage_mv <= BATTERY_CRITICAL_MV) {
        new_state = BATTERY_STATE_CRITICAL;
    } else if (voltage_mv <= BATTERY_LOW_MV) {
        new_state = BATTERY_STATE_LOW;
    } else {
        new_state = BATTERY_STATE_NORMAL;
    }

    // Handle state changes
    if (new_state != battery_state) {
        battery_state = new_state;

        switch (battery_state) {
            case BATTERY_STATE_LOW:
                event_logger_add("Battery low!", true);
                gpio_set_level(LED_STATUS, 0);
                break;

            case BATTERY_STATE_CRITICAL:
                event_logger_add("Battery critical! Connect charger immediately!", true);
                gpio_set_level(LED_ERROR, 1);
                // Force eco mode
                power_manager_set_mode(POWER_MODE_ECO);
                break;

            case BATTERY_STATE_CHARGING:
                event_logger_add("Battery charging", false);
                gpio_set_level(LED_STATUS, 1);
                gpio_set_level(LED_ERROR, 0);
                break;

            case BATTERY_STATE_FULL:
                event_logger_add("Battery fully charged", false);
                gpio_set_level(LED_STATUS, 1);
                gpio_set_level(LED_ERROR, 0);
                break;

            default:
                gpio_set_level(LED_STATUS, 1);
                gpio_set_level(LED_ERROR, 0);
                break;
        }
    }
}
