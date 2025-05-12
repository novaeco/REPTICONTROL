#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include "esp_err.h"
#include "lvgl.h"

// Display specifications
#define LCD_H_RES 1024
#define LCD_V_RES 600
#define LCD_PIXEL_CLOCK_HZ (18 * 1000 * 1000)
#define LCD_BITS_PER_PIXEL 16

// Backlight control
#define LCD_BK_LIGHT_ON_LEVEL 1
#define LCD_BK_LIGHT_OFF_LEVEL 0

// Display initialization
esp_err_t display_init(void);

// Backlight control
void display_set_backlight(bool on);
void display_set_brightness(uint8_t level); // 0-255

// Power management
void display_enter_sleep(void);
void display_exit_sleep(void);

// Screen rotation
void display_set_rotation(lv_disp_rot_t rotation);

// LVGL callbacks
void display_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);
void display_rounder_cb(lv_disp_drv_t *drv, lv_area_t *area);
void display_set_px_cb(lv_disp_drv_t *drv, uint8_t *buf, lv_coord_t buf_w,
                      lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa);

// DMA transfer completion callback
void display_dma_ready_cb(void);

#endif /* DISPLAY_DRIVER_H */