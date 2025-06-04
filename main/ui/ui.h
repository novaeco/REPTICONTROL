#ifndef UI_H
#define UI_H

#include "lvgl.h"
#include <stdbool.h>

// Screen IDs
typedef enum {
    SCREEN_DASHBOARD,
    SCREEN_CLIMATE,
    SCREEN_SCHEDULE,
    SCREEN_SYSTEM,
    SCREEN_LOGS,
    SCREEN_COUNT
} screen_t;

// UI initialization function
void ui_init(void);

// UI update function (to be called in a loop)
void ui_update(void);

// Switch to a specific screen with animation
void ui_switch_screen(screen_t screen);

// Get current active screen
screen_t ui_get_active_screen(void);

// Update UI with sensor data
void ui_update_sensor_data(float temperature, float humidity, float light);

// Update UI with system status
void ui_update_system_status(int battery_level, bool heating_on, bool cooling_on, 
                             bool humidifier_on, bool lighting_on);

// Add a log entry to the log screen
void ui_add_log_entry(const char* message, bool is_alert);

// Show alert message
void ui_show_alert(const char* title, const char* message);

#endif /* UI_H */
