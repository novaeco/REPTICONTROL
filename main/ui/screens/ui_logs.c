#include "ui_logs.h"
#include "../ui_helpers.h"
#include "esp_log.h"
#include <time.h>

static const char *TAG = "ui_logs";

// Store references
static lv_obj_t *log_list;
static int log_count = 0;

// Button handlers
static void clear_logs_cb(lv_event_t *e);

// Create the logs screen
lv_obj_t *ui_logs_create(void) {
    ESP_LOGI(TAG, "Creating logs screen");
    
    // Create screen container
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, lv_color_hex(0xF5F5F5), 0); // Light gray background
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    
    // Create header panel
    lv_obj_t *header = lv_obj_create(screen);
    lv_obj_set_size(header, LV_HOR_RES, 48);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_style(header, &style_header, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);
    
    // Add screen title to header
    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, "Event Logs");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, 16, 0);
    
    // Clear logs button
    lv_obj_t *clear_btn = lv_btn_create(header);
    lv_obj_set_size(clear_btn, 80, 32);
    lv_obj_align(clear_btn, LV_ALIGN_RIGHT_MID, -16, 0);
    lv_obj_set_style_bg_color(clear_btn, COLOR_ERROR, 0);
    lv_obj_add_event_cb(clear_btn, clear_logs_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *clear_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_label, "Clear");
    lv_obj_center(clear_label);
    
    // Main logs panel
    lv_obj_t *logs_panel = lv_obj_create(screen);
    lv_obj_set_size(logs_panel, LV_HOR_RES - 32, LV_VER_RES - 128);
    lv_obj_align(logs_panel, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_add_style(logs_panel, &style_card, 0);
    
    // Create log list
    log_list = lv_list_create(logs_panel);
    lv_obj_set_size(log_list, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(log_list, 8, 0);
    
    // Add some initial log entries for demonstration
    ui_logs_add_entry("System started", false);
    ui_logs_add_entry("Temperature sensor initialized", false);
    ui_logs_add_entry("Humidity sensor initialized", false);
    ui_logs_add_entry("Light sensor initialized", false);
    ui_logs_add_entry("All systems operational", false);
    
    return screen;
}

// Add an entry to the log
void ui_logs_add_entry(const char* message, bool is_alert) {
    if (!log_list) return;
    
    // Get current time
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    
    // Format time string
    char time_str[16];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", &timeinfo);
    
    // Create full message with timestamp
    char full_msg[128];
    snprintf(full_msg, sizeof(full_msg), "[%s] %s", time_str, message);
    
    // Add log entry
    lv_obj_t *list_btn;
    
    if (is_alert) {
        // Create alert entry with warning icon and red color
        list_btn = lv_list_add_btn(log_list, LV_SYMBOL_WARNING, full_msg);
        lv_obj_set_style_bg_color(list_btn, lv_color_hex(0xFFEBEE), 0); // Light red background
        lv_obj_t *label = lv_obj_get_child(list_btn, 1);
        lv_obj_set_style_text_color(label, COLOR_ERROR, 0);
    } else {
        // Create normal entry with info icon
        list_btn = lv_list_add_btn(log_list, LV_SYMBOL_FILE, full_msg);
    }
    
    // Add subtle styling to log entry
    lv_obj_set_style_bg_opa(list_btn, LV_OPA_20, 0);
    
    // Ensure newest logs appear at the top
    if (lv_list_get_btn_count(log_list) > 0) {
        // Move the new item to the top
        lv_obj_move_to_index(list_btn, 0);
    }
    
    // Limit maximum number of log entries to prevent memory issues
    if (log_count >= 50) {
        // Remove oldest entries
        lv_obj_t *last_btn = lv_obj_get_child(log_list, lv_obj_get_child_cnt(log_list) - 1);
        if (last_btn) {
            lv_obj_del(last_btn);
        }
    } else {
        log_count++;
    }
}

// Clear all logs
void ui_logs_clear(void) {
    if (log_list) {
        lv_obj_clean(log_list);
        log_count = 0;
        ui_logs_add_entry("Log cleared", false);
    }
}

// Button handlers
static void clear_logs_cb(lv_event_t *e) {
    ui_logs_clear();
}