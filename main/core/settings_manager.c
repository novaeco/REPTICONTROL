#include "settings_manager.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "event_logger.h"
#include <string.h>

static const char *TAG = "settings_manager";

// NVS handle
static nvs_handle_t settings_handle;

// Initialize settings manager
void settings_init(void) {
    ESP_LOGI(TAG, "Initializing settings manager");
    
    // Open NVS handle
    esp_err_t err = nvs_open("settings", NVS_READWRITE, &settings_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle: %s", esp_err_to_name(err));
        return;
    }
    
    // Load settings
    settings_load();
    
    event_logger_add("Settings manager initialized", false);
}

// Load all settings
void settings_load(void) {
    ESP_LOGI(TAG, "Loading settings from NVS");
    
    // For climate control, settings are loaded by the respective modules
    // through the get methods. This function is mainly for future use.
}

// Save all settings
void settings_save(void) {
    ESP_LOGI(TAG, "Saving settings to NVS");
    
    // Commit any pending writes
    esp_err_t err = nvs_commit(settings_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error committing settings: %s", esp_err_to_name(err));
        return;
    }
    
    event_logger_add("Settings saved", false);
}

// Get float value
float settings_get_float(const char* key, float default_value) {
    float value = default_value;
    
    // First try to read as float (newer entries)
    esp_err_t err = nvs_get_float(settings_handle, key, &value);
    
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        // If not found, try legacy format (int32 with fixed point)
        int32_t int_value;
        err = nvs_get_i32(settings_handle, key, &int_value);
        if (err == ESP_OK) {
            // Convert from fixed point (2 decimal places)
            value = (float)int_value / 100.0f;
        } else {
            // Use default value
            value = default_value;
        }
    }
    
    return value;
}

// Set float value
void settings_set_float(const char* key, float value) {
    esp_err_t err = nvs_set_float(settings_handle, key, value);
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error setting float for %s: %s", key, esp_err_to_name(err));
    }
}

// Get boolean value
bool settings_get_bool(const char* key, bool default_value) {
    uint8_t value;
    esp_err_t err = nvs_get_u8(settings_handle, key, &value);
    
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return default_value;
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error getting bool for %s: %s", key, esp_err_to_name(err));
        return default_value;
    }
    
    return (value != 0);
}

// Set boolean value
void settings_set_bool(const char* key, bool value) {
    esp_err_t err = nvs_set_u8(settings_handle, key, value ? 1 : 0);
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error setting bool for %s: %s", key, esp_err_to_name(err));
    }
}

// Get integer value
int settings_get_int(const char* key, int default_value) {
    int32_t value;
    esp_err_t err = nvs_get_i32(settings_handle, key, &value);
    
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return default_value;
    } else if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error getting int for %s: %s", key, esp_err_to_name(err));
        return default_value;
    }
    
    return (int)value;
}

// Set integer value
void settings_set_int(const char* key, int value) {
    esp_err_t err = nvs_set_i32(settings_handle, key, (int32_t)value);
    
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error setting int for %s: %s", key, esp_err_to_name(err));
    }
}

bool settings_has_key(const char* key) {
    int32_t dummy;
    esp_err_t err = nvs_get_i32(settings_handle, key, &dummy);
    return err != ESP_ERR_NVS_NOT_FOUND;

}

}
 main
