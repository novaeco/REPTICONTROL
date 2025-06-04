#ifndef UI_TIMER_H
#define UI_TIMER_H

#include "esp_err.h"

// Initialize LVGL timer
esp_err_t ui_timer_init(void);

// Stop LVGL timer
void ui_timer_stop(void);

#endif /* UI_TIMER_H */
