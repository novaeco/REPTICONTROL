#include "app_main.h"
#include "drivers/display_driver.h"
#include "drivers/display_config.h"
#include "drivers/touch_driver.h"
#include "ui/screens/ui_first_setup.h"
#include "ui/ui.h"
#include "core/climate_controller.h"
#include "core/data_simulator.h"
#include "core/event_logger.h"
#include "core/system_monitor.h"
#include "core/settings_manager.h"
#include "core/power_manager.h"
#include "core/network_manager.h"
#include "core/watchdog_manager.h"
#include "utils/rtc_manager.h"
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "app_main";

// Forward declarations for task functions
static void ui_task(void *pvParameter);
static void sensor_simulator_task(void *pvParameter);
static void climate_control_task(void *pvParameter);
static void system_monitor_task(void *pvParameter);
static void power_management_task(void *pvParameter);
static void network_task(void *pvParameter);

// Initialization and main entry point
void repticontrol_main(void) {
    ESP_LOGI(TAG, "Initializing ReptiControl application");

    // Initialize components
    settings_init();
    display_config_init();
    display_init();
    touch_init();
    rtc_init();
    event_logger_init();
    climate_controller_init();
    data_simulator_init();
    system_monitor_init();
    power_manager_init();
    network_manager_init();
    watchdog_manager_init();

    // Initialize the UI (with splash screen)
    ui_init();

    // First-run setup if necessary
    if (!settings_has_display_type()) {
        ui_first_setup_create();
        while (!ui_first_setup_is_complete()) {
            ui_update();
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        display_config_apply(display_config_get_type());
    }

    // Register tasks with watchdog
    watchdog_manager_register_task("ui_task", 2000);
    watchdog_manager_register_task("simulator_task", 3000);
    watchdog_manager_register_task("climate_task", 2000);
    watchdog_manager_register_task("monitor_task", 4000);
    watchdog_manager_register_task("power_task", 3000);
    watchdog_manager_register_task("network_task", 5000);

    // Create tasks
    xTaskCreatePinnedToCore(ui_task, "ui_task", 4096, NULL, 5, NULL, 1);
    xTaskCreate(sensor_simulator_task, "simulator_task", 2048, NULL, 3, NULL);
    xTaskCreate(climate_control_task, "climate_task", 2048, NULL, 4, NULL);
    xTaskCreate(system_monitor_task, "monitor_task", 2048, NULL, 2, NULL);
    xTaskCreate(power_management_task, "power_task", 2048, NULL, 2, NULL);
    xTaskCreate(network_task, "network_task", 4096, NULL, 1, NULL);

    ESP_LOGI(TAG, "ReptiControl started successfully");
}

// UI task - handles all GUI updates
static void ui_task(void *pvParameter) {
    ESP_LOGI(TAG, "UI task started");

    while (1) {
        // Process UI events
        ui_update();

        // Feed watchdog
        watchdog_manager_feed("ui_task");

        // Short delay to prevent CPU hogging
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// Simulates sensor data
static void sensor_simulator_task(void *pvParameter) {
    ESP_LOGI(TAG, "Sensor simulator task started");

    while (1) {
        // Update simulated sensor readings
        data_simulator_update();

        // Update BLE characteristics with new sensor values
        network_manager_ble_update_sensors(
            data_simulator_get_temperature(),
            data_simulator_get_humidity(),
            data_simulator_get_light()
        );

        // Feed watchdog
        watchdog_manager_feed("simulator_task");

        // Delay between updates (1 second)
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Climate control execution task
static void climate_control_task(void *pvParameter) {
    ESP_LOGI(TAG, "Climate control task started");

    while (1) {
        // Update climate controls based on current settings and schedule
        climate_controller_update();

        // Feed watchdog
        watchdog_manager_feed("climate_task");

        // Delay between control updates (500ms)
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// System monitoring task
static void system_monitor_task(void *pvParameter) {
    ESP_LOGI(TAG, "System monitor task started");

    while (1) {
        // Update system status (battery, memory, etc.)
        system_monitor_update();

        // Feed watchdog
        watchdog_manager_feed("monitor_task");

        // Delay between system updates (2 seconds)
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

// Power management task
static void power_management_task(void *pvParameter) {
    ESP_LOGI(TAG, "Power management task started");

    while (1) {
        // Update power management status
        power_manager_update();

        // Feed watchdog
        watchdog_manager_feed("power_task");

        // Delay between updates (1 second)
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Network management task
static void network_task(void *pvParameter) {
    ESP_LOGI(TAG, "Network task started");

    // Load Wi-Fi credentials from settings
    rc_wifi_config_t wifi_config = {0};
    if (!network_manager_load_wifi_credentials(wifi_config.ssid, sizeof(wifi_config.ssid),
                                               wifi_config.password, sizeof(wifi_config.password),
                                               &wifi_config.mode)) {
        // Fallback to AP mode
        strncpy(wifi_config.ssid, "ReptiControl", sizeof(wifi_config.ssid));
        strncpy(wifi_config.password, "repticontrol123", sizeof(wifi_config.password));
        wifi_config.mode = RC_WIFI_MODE_AP;
    }

    network_manager_wifi_start(&wifi_config);
    network_manager_ble_start();

    while (1) {
        // Feed watchdog
        watchdog_manager_feed("network_task");

        // Network management loop
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
