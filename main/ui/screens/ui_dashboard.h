#ifndef UI_DASHBOARD_H
#define UI_DASHBOARD_H

#include "lvgl.h"
#include <stdbool.h>

// Create the dashboard screen
lv_obj_t *ui_dashboard_create(void);

// Update dashboard with sensor data
void ui_dashboard_update_sensors(float temperature, float humidity, float light);

// Update dashboard with system status
void ui_dashboard_update_status(int battery_level, bool heating_on, bool cooling_on, 
                              bool humidifier_on, bool lighting_on);

// Update time display
void ui_dashboard_update_time(int hour, int minute);

// Update reptile animation based on conditions
void ui_dashboard_update_reptile_animation(float temperature, float humidity, float light);

#endif /* UI_DASHBOARD_H */