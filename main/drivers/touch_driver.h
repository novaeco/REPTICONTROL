#ifndef TOUCH_DRIVER_H
#define TOUCH_DRIVER_H

#include "esp_err.h"

// Touch driver initialization
esp_err_t touch_init(void);

// Read touch data
void touch_read(void);

// LVGL touch read callback
void touch_read_cb(struct _lv_indev_drv_t *indev_drv, lv_indev_data_t *data);

#endif /* TOUCH_DRIVER_H */
