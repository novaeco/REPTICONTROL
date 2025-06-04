#ifndef UI_LOGS_H
#define UI_LOGS_H

#include "lvgl.h"
#include <stdbool.h>

// Create the logs screen
lv_obj_t *ui_logs_create(void);

// Add an entry to the log
void ui_logs_add_entry(const char* message, bool is_alert);

// Clear all logs
void ui_logs_clear(void);

#endif /* UI_LOGS_H */
