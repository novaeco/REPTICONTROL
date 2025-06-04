#include "display_driver.h"
#include "pin_mapping.h"
#include "display_config.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_pm.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_heap_caps.h"
#include <string.h>
#include "lvgl.h"

static const char *TAG = "display_driver";

// Display handles
static esp_lcd_panel_handle_t panel_handle = NULL;
static esp_lcd_panel_io_handle_t io_handle = NULL;
static lv_disp_drv_t disp_drv;
static SemaphoreHandle_t lcd_mutex = NULL;

// DMA buffers
static lv_color_t *buf1 = NULL;
static lv_color_t *buf2 = NULL;
static lv_disp_draw_buf_t disp_buf;

// Display configuration
static esp_lcd_rgb_panel_config_t panel_config;

esp_err_t display_init(void) {
    ESP_LOGI(TAG, "Initializing display driver");

    // Create mutex for display access
    lcd_mutex = xSemaphoreCreateMutex();
    if (!lcd_mutex) {
        ESP_LOGE(TAG, "Failed to create LCD mutex");
        return ESP_FAIL;
    }

    // Allocate frame buffers in PSRAM
    size_t fb_size = LCD_H_RES * 40 * sizeof(lv_color_t); // Double buffering with partial updates
    buf1 = heap_caps_malloc(fb_size, MALLOC_CAP_SPIRAM);
    buf2 = heap_caps_malloc(fb_size, MALLOC_CAP_SPIRAM);

    if (!buf1 || !buf2) {
        ESP_LOGE(TAG, "Failed to allocate frame buffers");
        return ESP_FAIL;
    }

    // Configure panel parameters based on selected display
    memset(&panel_config, 0, sizeof(panel_config));
    panel_config.data_width = 16;
    panel_config.psram_trans_align = 64;
    panel_config.clk_src = LCD_CLK_SRC_PLL160M;
    panel_config.disp_gpio_num = LCD_DISP_EN_GPIO;
    panel_config.pclk_gpio_num = LCD_PCLK_GPIO;
    panel_config.vsync_gpio_num = LCD_VSYNC_GPIO;
    panel_config.hsync_gpio_num = LCD_HSYNC_GPIO;
    panel_config.de_gpio_num = LCD_DE_GPIO;
    int data_pins[] = { LCD_D0_GPIO, LCD_D1_GPIO, LCD_D2_GPIO, LCD_D3_GPIO,
                        LCD_D4_GPIO, LCD_D5_GPIO, LCD_D6_GPIO, LCD_D7_GPIO,
                        LCD_D8_GPIO, LCD_D9_GPIO, LCD_D10_GPIO, LCD_D11_GPIO,
                        LCD_D12_GPIO, LCD_D13_GPIO, LCD_D14_GPIO, LCD_D15_GPIO };
    memcpy(panel_config.data_gpio_nums, data_pins, sizeof(data_pins));
    panel_config.timings.pclk_hz = LCD_PIXEL_CLOCK_HZ;
    panel_config.timings.h_res = LCD_H_RES;
    panel_config.timings.v_res = LCD_V_RES;
    panel_config.timings.hsync_pulse_width = 20;
    panel_config.timings.hsync_back_porch = 140;
    panel_config.timings.hsync_front_porch = 160;
    panel_config.timings.vsync_pulse_width = 3;
    panel_config.timings.vsync_back_porch = 20;
    panel_config.timings.vsync_front_porch = 12;
    panel_config.timings.flags.pclk_active_neg = 1;
    panel_config.flags.fb_in_psram = 1;
    panel_config.flags.refresh_on_demand = 0;
    panel_config.flags.bb_invalidate_cache = 1;

    // Initialize RGB LCD panel
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));

    // Configure backlight GPIO
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << LCD_BK_LIGHT_GPIO
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    // Turn on backlight
    gpio_set_level(LCD_BK_LIGHT_GPIO, LCD_BK_LIGHT_ON_LEVEL);

    // Initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, LCD_H_RES * 40);

    // Register display driver
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LCD_H_RES;
    disp_drv.ver_res = LCD_V_RES;
    disp_drv.flush_cb = display_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.rounder_cb = display_rounder_cb;
    disp_drv.set_px_cb = display_set_px_cb;
    disp_drv.full_refresh = 0;
    disp_drv.direct_mode = 1;
    lv_disp_drv_register(&disp_drv);

    ESP_LOGI(TAG, "Display initialized successfully");
    return ESP_OK;
}

void display_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map) {
    if (xSemaphoreTake(lcd_mutex, portMAX_DELAY) == pdTRUE) {
        esp_lcd_panel_draw_bitmap(panel_handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_map);
        xSemaphoreGive(lcd_mutex);
    }
    lv_disp_flush_ready(drv);
}

void display_rounder_cb(lv_disp_drv_t *drv, lv_area_t *area) {
    // Optimize area for 16-bit RGB565
    area->x1 = area->x1 & ~1;
    area->x2 = area->x2 | 1;
}

void display_set_px_cb(lv_disp_drv_t *drv, uint8_t *buf, lv_coord_t buf_w,
                      lv_coord_t x, lv_coord_t y, lv_color_t color, lv_opa_t opa) {
    // Direct pixel manipulation if needed
    if (opa >= LV_OPA_MAX) {
        ((lv_color_t *)buf)[y * buf_w + x] = color;
    } else {
        lv_color_t mix_color = lv_color_mix(color, ((lv_color_t *)buf)[y * buf_w + x], opa);
        ((lv_color_t *)buf)[y * buf_w + x] = mix_color;
    }
}

void display_set_backlight(bool on) {
    gpio_set_level(LCD_BK_LIGHT_GPIO, on ? LCD_BK_LIGHT_ON_LEVEL : LCD_BK_LIGHT_OFF_LEVEL);
}

void display_set_brightness(uint8_t level) {
    // Implement PWM-based brightness control if hardware supports it
    // For now, just on/off control
    display_set_backlight(level > 0);
}

void display_enter_sleep(void) {
    if (xSemaphoreTake(lcd_mutex, portMAX_DELAY) == pdTRUE) {
        display_set_backlight(false);
        esp_lcd_panel_disp_off(panel_handle, true);
        xSemaphoreGive(lcd_mutex);
    }
}

void display_exit_sleep(void) {
    if (xSemaphoreTake(lcd_mutex, portMAX_DELAY) == pdTRUE) {
        esp_lcd_panel_disp_off(panel_handle, false);
        display_set_backlight(true);
        xSemaphoreGive(lcd_mutex);
    }
}

void display_set_rotation(lv_disp_rot_t rotation) {
    if (xSemaphoreTake(lcd_mutex, portMAX_DELAY) == pdTRUE) {
        esp_lcd_panel_swap_xy(panel_handle, rotation == LV_DISP_ROT_90 || rotation == LV_DISP_ROT_270);
        esp_lcd_panel_mirror(panel_handle, rotation == LV_DISP_ROT_180 || rotation == LV_DISP_ROT_270,
                           rotation == LV_DISP_ROT_180 || rotation == LV_DISP_ROT_90);
        xSemaphoreGive(lcd_mutex);
    }
}

void display_dma_ready_cb(void) {
    BaseType_t high_task_awoken = pdFALSE;
    xSemaphoreGiveFromISR(lcd_mutex, &high_task_awoken);
    if (high_task_awoken) {
        portYIELD_FROM_ISR();
    }
}