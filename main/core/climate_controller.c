#include "climate_controller.h"
#include "data_simulator.h"
#include "event_logger.h"
#include "esp_log.h"
#include <math.h>

static const char *TAG = "climate_controller";

// Target values
static float temp_target = 25.0f;
static float humidity_target = 50.0f;
static float light_target = 75.0f;

// System state
static bool heating_enabled = true;
static bool cooling_enabled = true;
static bool humidifier_enabled = true;
static bool lighting_enabled = true;

// Active status
static bool heating_active = false;
static bool cooling_active = false;
static bool humidifier_active = false;
static bool lighting_active = false;

// Hysteresis values to prevent rapid cycling
static const float TEMP_HYSTERESIS = 1.0f;    // ±1°C
static const float HUMIDITY_HYSTERESIS = 5.0f; // ±5%
static const float LIGHT_HYSTERESIS = 5.0f;    // ±5%

// Forward declarations
static void update_heating_cooling(float current_temp);
static void update_humidifier(float current_humidity);
static void update_lighting(float current_light);

// Initialize the climate controller
void climate_controller_init(void) {
    ESP_LOGI(TAG, "Initializing climate controller");
    
    // Set initial target values
    temp_target = 25.0f;
    humidity_target = 50.0f;
    light_target = 75.0f;
    
    // Enable all systems by default
    heating_enabled = true;
    cooling_enabled = true;
    humidifier_enabled = true;
    lighting_enabled = true;
    
    // All systems start inactive
    heating_active = false;
    cooling_active = false;
    humidifier_active = false;
    lighting_active = false;
    
    event_logger_add("Climate controller initialized", false);
}

// Update climate control logic
void climate_controller_update(void) {
    // Get current sensor values
    float current_temp = data_simulator_get_temperature();
    float current_humidity = data_simulator_get_humidity();
    float current_light = data_simulator_get_light();
    
    // Update each system
    update_heating_cooling(current_temp);
    update_humidifier(current_humidity);
    update_lighting(current_light);
}

// Control logic for heating and cooling
static void update_heating_cooling(float current_temp) {
    // Check if heating should be activated
    if (heating_enabled && !heating_active && current_temp < (temp_target - TEMP_HYSTERESIS)) {
        heating_active = true;
        cooling_active = false; // Prevent both systems running simultaneously
        event_logger_add("Heating activated", false);
    }
    // Check if heating should be deactivated
    else if (heating_active && current_temp > (temp_target + TEMP_HYSTERESIS)) {
        heating_active = false;
        event_logger_add("Heating deactivated", false);
    }
    
    // Check if cooling should be activated
    if (cooling_enabled && !cooling_active && current_temp > (temp_target + TEMP_HYSTERESIS)) {
        cooling_active = true;
        heating_active = false; // Prevent both systems running simultaneously
        event_logger_add("Cooling activated", false);
    }
    // Check if cooling should be deactivated
    else if (cooling_active && current_temp < (temp_target - TEMP_HYSTERESIS)) {
        cooling_active = false;
        event_logger_add("Cooling deactivated", false);
    }
    
    // Apply influence to the simulated environment
    if (heating_active) {
        data_simulator_apply_heating();
    }
    
    if (cooling_active) {
        data_simulator_apply_cooling();
    }
}

// Control logic for humidifier
static void update_humidifier(float current_humidity) {
    // Check if humidifier should be activated
    if (humidifier_enabled && !humidifier_active && current_humidity < (humidity_target - HUMIDITY_HYSTERESIS)) {
        humidifier_active = true;
        event_logger_add("Humidifier activated", false);
    }
    // Check if humidifier should be deactivated
    else if (humidifier_active && current_humidity > (humidity_target + HUMIDITY_HYSTERESIS)) {
        humidifier_active = false;
        event_logger_add("Humidifier deactivated", false);
    }
    
    // Apply influence to the simulated environment
    if (humidifier_active) {
        data_simulator_apply_humidifier();
    }
}

// Control logic for lighting
static void update_lighting(float current_light) {
    // For lighting, we try to maintain the exact level
    if (lighting_enabled) {
        // Only toggle state if we're significantly off target
        if (!lighting_active && fabs(current_light - light_target) > LIGHT_HYSTERESIS) {
            lighting_active = true;
            event_logger_add("Lighting adjusted", false);
        }
    } else if (lighting_active) {
        lighting_active = false;
        event_logger_add("Lighting disabled", false);
    }
    
    // Apply target to simulator
    if (lighting_active) {
        data_simulator_set_light_target(light_target);
    } else {
        data_simulator_set_light_target(0);
    }
}

// Set target temperature
void climate_controller_set_temp_target(float temp) {
    if (temp >= 15.0f && temp <= 40.0f) {
        temp_target = temp;
        ESP_LOGI(TAG, "Temperature target set to %.1f°C", temp_target);
        event_logger_add_fmt("Temperature target set to %.1f°C", false, temp_target);
    }
}

// Set target humidity
void climate_controller_set_humidity_target(float humidity) {
    if (humidity >= 20.0f && humidity <= 90.0f) {
        humidity_target = humidity;
        ESP_LOGI(TAG, "Humidity target set to %.1f%%", humidity_target);
        event_logger_add_fmt("Humidity target set to %.1f%%", false, humidity_target);
    }
}

// Set target light level
void climate_controller_set_light_target(float light) {
    if (light >= 0.0f && light <= 100.0f) {
        light_target = light;
        ESP_LOGI(TAG, "Light target set to %.1f%%", light_target);
        event_logger_add_fmt("Light target set to %.1f%%", false, light_target);
    }
}

// Toggle heating system
void climate_controller_set_heating(bool enable) {
    heating_enabled = enable;
    if (!enable && heating_active) {
        heating_active = false;
    }
    ESP_LOGI(TAG, "Heating system %s", enable ? "enabled" : "disabled");
    event_logger_add_fmt("Heating system %s", false, enable ? "enabled" : "disabled");
}

// Toggle cooling system
void climate_controller_set_cooling(bool enable) {
    cooling_enabled = enable;
    if (!enable && cooling_active) {
        cooling_active = false;
    }
    ESP_LOGI(TAG, "Cooling system %s", enable ? "enabled" : "disabled");
    event_logger_add_fmt("Cooling system %s", false, enable ? "enabled" : "disabled");
}

// Toggle humidifier
void climate_controller_set_humidifier(bool enable) {
    humidifier_enabled = enable;
    if (!enable && humidifier_active) {
        humidifier_active = false;
    }
    ESP_LOGI(TAG, "Humidifier %s", enable ? "enabled" : "disabled");
    event_logger_add_fmt("Humidifier %s", false, enable ? "enabled" : "disabled");
}

// Toggle lighting
void climate_controller_set_lighting(bool enable) {
    lighting_enabled = enable;
    if (!enable && lighting_active) {
        lighting_active = false;
    }
    ESP_LOGI(TAG, "Lighting %s", enable ? "enabled" : "disabled");
    event_logger_add_fmt("Lighting %s", false, enable ? "enabled" : "disabled");
}

// Get current target temperature
float climate_controller_get_temp_target(void) {
    return temp_target;
}

// Get current target humidity
float climate_controller_get_humidity_target(void) {
    return humidity_target;
}

// Get current target light level
float climate_controller_get_light_target(void) {
    return light_target;
}

// Get heating system status
bool climate_controller_is_heating_on(void) {
    return heating_active;
}

// Get cooling system status
bool climate_controller_is_cooling_on(void) {
    return cooling_active;
}

// Get humidifier status
bool climate_controller_is_humidifier_on(void) {
    return humidifier_active;
}

// Get lighting status
bool climate_controller_is_lighting_on(void) {
    return lighting_active;
}