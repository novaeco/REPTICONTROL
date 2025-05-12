#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include "esp_err.h"

// Display initialization
esp_err_t display_init(void);

// Flush function for LVGL
void display_flush_cb(struct _lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);

#endif /* DISPLAY_DRIVER_H */