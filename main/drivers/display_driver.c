#include "display_driver.h"
#include "pin_mapping.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"
#include "esp_heap_caps.h"

static const char *TAG = "display_driver";

// Display buffer
static lv_disp_draw_buf_t disp_buf;
static lv_color_t *buf1;
static lv_color_t *buf2;

// Display driver
static lv_disp_drv_t disp_drv;
static esp_lcd_panel_handle_t panel_handle = NULL;

// LCD configuration
#define LCD_PIXEL_CLOCK_HZ     (18 * 1000 * 1000)
#define LCD_BK_LIGHT_ON_LEVEL  1
#define LCD_BK_LIGHT_OFF_LEVEL !LCD_BK_LIGHT_ON_LEVEL

esp_err_t display_init(void) {
    ESP_LOGI(TAG, "Initializing display driver");
    
    // Allocate display buffers in PSRAM
    buf1 = heap_caps_malloc(1024 * 30 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    buf2 = heap_caps_malloc(1024 * 30 * sizeof(lv_color_t), MALLOC_CAP_SPIRAM);
    
    if (!buf1 || !buf2) {
        ESP_LOGE(TAG, "Failed to allocate display buffers");
        return ESP_FAIL;
    }
    
    // Configure LCD GPIO
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << LCD_BK_LIGHT_GPIO
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
    
    // Configure RGB timing
    esp_lcd_rgb_panel_config_t panel_config = {
        .data_width = 16,
        .psram_trans_align = 64,
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .disp_gpio_num = LCD_DE_PIN,
        .pclk_gpio_num = LCD_PCLK_PIN,
        .vsync_gpio_num = LCD_VSYNC_PIN,
        .hsync_gpio_num = LCD_HSYNC_PIN,
        .de_gpio_num = LCD_DE_PIN,
        .data_gpio_nums = {
            LCD_B0_PIN, LCD_B1_PIN, LCD_B2_PIN, LCD_B3_PIN, LCD_B4_PIN,
            LCD_G0_PIN, LCD_G1_PIN, LCD_G2_PIN, LCD_G3_PIN, LCD_G4_PIN, LCD_G5_PIN,
            LCD_R0_PIN, LCD_R1_PIN, LCD_R2_PIN, LCD_R3_PIN, LCD_R4_PIN,
        },
        .timings = {
            .pclk_hz = LCD_PIXEL_CLOCK_HZ,
            .h_res = 1024,
            .v_res = 600,
            .hsync_pulse_width = 20,
            .hsync_back_porch = 140,
            .hsync_front_porch = 160,
            .vsync_pulse_width = 3,
            .vsync_back_porch = 20,
            .vsync_front_porch = 12,
            .flags = {
                .pclk_active_neg = 1,
            },
        },
        .flags = {
            .fb_in_psram = 1,
        },
    };
    
    // Initialize LCD panel
    ESP_ERROR_CHECK(esp_lcd_new_rgb_panel(&panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    
    // Turn on backlight
    gpio_set_level(LCD_BK_LIGHT_GPIO, LCD_BK_LIGHT_ON_LEVEL);
    
    // Initialize LVGL
    lv_init();
    
    // Initialize buffer
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, 1024 * 30);
    
    // Register the display driver
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 1024;
    disp_drv.ver_res = 600;
    disp_drv.flush_cb = display_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    lv_disp_drv_register(&disp_drv);
    
    ESP_LOGI(TAG, "Display driver initialized");
    return ESP_OK;
}

void display_flush_cb(struct _lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    esp_lcd_panel_draw_bitmap(panel_handle, area->x1, area->y1, area->x2 + 1, area->y2 + 1, color_p);
    lv_disp_flush_ready(disp_drv);
}