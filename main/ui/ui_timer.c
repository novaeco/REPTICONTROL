#include "ui_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h"

static const char *TAG = "ui_timer";

// LVGL timer handle
static lv_timer_t *lvgl_timer = NULL;

// Timer callback for LVGL
static void lvgl_timer_cb(lv_timer_t *timer) {
    lv_tick_inc(portTICK_PERIOD_MS);
    lv_timer_handler();
}

// Initialize LVGL timer
esp_err_t ui_timer_init(void) {
    ESP_LOGI(TAG, "Initializing UI timer");

    // Create LVGL timer
    lvgl_timer = lv_timer_create(lvgl_timer_cb, portTICK_PERIOD_MS, NULL);
    if (!lvgl_timer) {
        ESP_LOGE(TAG, "Failed to create LVGL timer");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "UI timer initialized");
    return ESP_OK;
}

// Stop LVGL timer
void ui_timer_stop(void) {
    if (lvgl_timer) {
        lv_timer_del(lvgl_timer);
        lvgl_timer = NULL;
    }
}
