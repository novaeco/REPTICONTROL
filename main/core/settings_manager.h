#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include <stdbool.h>

// Settings keys
#define SETTINGS_KEY_TEMP_TARGET "temp_target"
#define SETTINGS_KEY_HUMIDITY_TARGET "humidity_target"
#define SETTINGS_KEY_LIGHT_TARGET "light_target"
#define SETTINGS_KEY_HEATING_ENABLED "heating_enabled"
#define SETTINGS_KEY_COOLING_ENABLED "cooling_enabled"
#define SETTINGS_KEY_HUMIDIFIER_ENABLED "humidifier_enabled"
#define SETTINGS_KEY_LIGHTING_ENABLED "lighting_enabled"

// Initialize settings manager
void settings_init(void);

// Load all settings
void settings_load(void);

// Save all settings
void settings_save(void);

// Get float value
float settings_get_float(const char* key, float default_value);

// Set float value
void settings_set_float(const char* key, float value);

// Get boolean value
bool settings_get_bool(const char* key, bool default_value);

// Set boolean value
void settings_set_bool(const char* key, bool value);

// Get integer value
int settings_get_int(const char* key, int default_value);

// Set integer value
void settings_set_int(const char* key, int value);

// Check if a key exists in NVS
bool settings_has_key(const char* key);

// Helper to check if display_type stored
static inline bool settings_has_display_type(void) {
    return settings_has_key("display_type");
}


#endif /* SETTINGS_MANAGER_H */

#endif /* SETTINGS_MANAGER_H */
 main
