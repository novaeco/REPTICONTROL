#ifndef UI_SYSTEM_H
#define UI_SYSTEM_H

#include "lvgl.h"
#include <stdbool.h>

// Create the system status screen
lv_obj_t *ui_system_create(void);

// Update system status
void ui_system_update_status(int battery_level, bool heating_on, bool cooling_on, 
                           bool humidifier_on, bool lighting_on);

// Update memory and CPU usage stats
void ui_system_update_stats(int cpu_usage, int memory_usage);

// Simulate system reboot
void ui_system_reboot(void);

// Simulate system shutdown
void ui_system_shutdown(void);

#endif /* UI_SYSTEM_H */