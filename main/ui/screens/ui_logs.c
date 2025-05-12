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
static void filter_logs_cb(lv_event_t *e);

// Create the logs screen
lv_obj_t *ui_logs_create(void) {
    ESP_LOGI(TAG, "Creating logs screen");
    
    // Create screen container
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(screen, COLOR_BACKGROUND, 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    
    // Create header panel
    lv_obj_t *header = lv_obj_create(screen);
    lv_obj_set_size(header, LV_HOR_RES, TOUCH_TARGET_MIN);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_style(header, &style_header, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);
    
    // Add screen title to header
    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, "Event Logs");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, GRID_UNIT * 2, 0);
    
    // Add action buttons to header
    lv_obj_t *btn_container = lv_obj_create(header);
    lv_obj_set_size(btn_container, TOUCH_TARGET_MIN * 5, TOUCH_TARGET_MIN);
    lv_obj_align(btn_container, LV_ALIGN_RIGHT_MID, -GRID_UNIT, 0);
    lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_container, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(btn_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btn_container, 0, 0);
    lv_obj_set_style_pad_gap(btn_container, GRID_UNIT, 0);
    
    // Filter button
    lv_obj_t *filter_btn = lv_btn_create(btn_container);
    lv_obj_set_size(filter_btn, TOUCH_TARGET_MIN, TOUCH_TARGET_MIN);
    lv_obj_add_style(filter_btn, &style_btn, 0);
    lv_obj_set_style_bg_color(filter_btn, COLOR_PRIMARY_DARK, 0);
    lv_obj_add_event_cb(filter_btn, filter_logs_cb, LV_EVENT_CLICKED, NULL);
    add_ripple_effect(filter_btn);
    
    lv_obj_t *filter_label = lv_label_create(filter_btn);
    lv_label_set_text(filter_label, LV_SYMBOL_LIST);
    lv_obj_center(filter_label);
    
    // Clear button
    lv_obj_t *clear_btn = lv_btn_create(btn_container);
    lv_obj_set_size(clear_btn, TOUCH_TARGET_MIN, TOUCH_TARGET_MIN);
    lv_obj_set_style_bg_color(clear_btn, COLOR_ERROR, 0);
    lv_obj_add_event_cb(clear_btn, clear_logs_cb, LV_EVENT_CLICKED, NULL);
    add_ripple_effect(clear_btn);
    
    lv_obj_t *clear_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_label, LV_SYMBOL_TRASH);
    lv_obj_center(clear_label);
    
    // Create main logs panel
    lv_obj_t *logs_panel = create_card(screen, NULL, LV_HOR_RES - SCREEN_PADDING * 2, 
                                     LV_VER_RES - TOUCH_TARGET_MIN - SCREEN_PADDING * 2);
    lv_obj_align(logs_panel, LV_ALIGN_TOP_MID, 0, TOUCH_TARGET_MIN + GRID_UNIT);
    
    // Create log list
    log_list = lv_list_create(logs_panel);
    lv_obj_set_size(log_list, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(log_list, GRID_UNIT, 0);
    lv_obj_set_style_pad_row(log_list, GRID_UNIT / 2, 0);
    
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
    
    // Create log entry container
    lv_obj_t *entry = lv_obj_create(log_list);
    lv_obj_set_size(entry, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(entry, GRID_UNIT, 0);
    lv_obj_set_style_bg_color(entry, is_alert ? lv_color_hex(0xFFF3E0) : lv_color_hex(0xF5F5F5), 0);
    lv_obj_set_style_bg_opa(entry, LV_OPA_40, 0);
    lv_obj_set_style_radius(entry, BORDER_RADIUS / 2, 0);
    add_ripple_effect(entry);
    
    // Create badge for alert/info icon
    lv_obj_t *icon_badge = create_badge(entry, 
                                      is_alert ? LV_SYMBOL_WARNING : LV_SYMBOL_FILE,
                                      is_alert ? COLOR_WARNING : COLOR_PRIMARY);
    lv_obj_align(icon_badge, LV_ALIGN_LEFT_MID, 0, 0);
    
    // Time label
    lv_obj_t *time_label = lv_label_create(entry);
    lv_label_set_text(time_label, time_str);
    lv_obj_add_style(time_label, &style_text_muted, 0);
    lv_obj_align(time_label, LV_ALIGN_LEFT_MID, TOUCH_TARGET_MIN * 1.2, 0);
    
    // Message label
    lv_obj_t *msg_label = lv_label_create(entry);
    lv_label_set_text(msg_label, message);
    lv_obj_set_style_text_color(msg_label, is_alert ? COLOR_ERROR : COLOR_TEXT, 0);
    lv_obj_align(msg_label, LV_ALIGN_LEFT_MID, TOUCH_TARGET_MIN * 2.5, 0);
    
    // Ensure newest logs appear at the top
    if (lv_obj_get_child_cnt(log_list) > 0) {
        lv_obj_move_to_index(entry, 0);
    }
    
    // Limit maximum number of log entries
    if (log_count >= 50) {
        lv_obj_t *last_entry = lv_obj_get_child(log_list, lv_obj_get_child_cnt(log_list) - 1);
        if (last_entry) {
            lv_obj_del_anim(last_entry, LV_ANIM_OFF);
        }
    } else {
        log_count++;
    }
}

// Clear all logs
void ui_logs_clear(void) {
    if (!log_list) return;
    
    // Show confirmation dialog
    static const char *btns[] = {"Yes", "No", ""};
    lv_obj_t *mbox = lv_msgbox_create(NULL, "Clear Logs", 
                                     "Are you sure you want to clear all logs?", btns, false);
    lv_obj_add_style(mbox, &style_alert_box, 0);
    lv_obj_center(mbox);
    
    lv_obj_add_event_cb(mbox, [](lv_event_t *e) {
        const char *btn_text = lv_msgbox_get_active_btn_text(lv_event_get_current_target(e));
        if (strcmp(btn_text, "Yes") == 0) {
            // Clear logs with fade-out animation
            lv_obj_t *list = log_list;
            uint32_t child_count = lv_obj_get_child_cnt(list);
            
            for (uint32_t i = 0; i < child_count; i++) {
                lv_obj_t *child = lv_obj_get_child(list, i);
                lv_obj_fade_out(child, 300, i * 50);
            }
            
            // Reset after animation
            lv_timer_t *timer = lv_timer_create([](lv_timer_t *t) {
                lv_obj_clean(log_list);
                log_count = 0;
                ui_logs_add_entry("Log cleared", false);
            }, 500 + child_count * 50, NULL);
            lv_timer_set_repeat_count(timer, 1);
        }
        lv_msgbox_close(lv_event_get_current_target(e));
    }, LV_EVENT_VALUE_CHANGED, NULL);
}

// Filter logs
static void filter_logs_cb(lv_event_t *e) {
    static const char *btns[] = {"All", "Alerts", "Info", "Close", ""};
    
    lv_obj_t *mbox = lv_msgbox_create(NULL, "Filter Logs", "Select log type to display:", btns, false);
    lv_obj_add_style(mbox, &style_alert_box, 0);
    lv_obj_center(mbox);
    
    lv_obj_add_event_cb(mbox, [](lv_event_t *e) {
        const char *btn_text = lv_msgbox_get_active_btn_text(lv_event_get_current_target(e));
        
        if (strcmp(btn_text, "Close") != 0) {
            // Apply visual filter effect
            uint32_t child_count = lv_obj_get_child_cnt(log_list);
            
            for (uint32_t i = 0; i < child_count; i++) {
                lv_obj_t *entry = lv_obj_get_child(log_list, i);
                bool is_alert = lv_obj_get_style_bg_color(entry, 0).full == lv_color_hex(0xFFF3E0).full;
                
                if (strcmp(btn_text, "All") == 0 ||
                    (strcmp(btn_text, "Alerts") == 0 && is_alert) ||
                    (strcmp(btn_text, "Info") == 0 && !is_alert)) {
                    lv_obj_clear_flag(entry, LV_OBJ_FLAG_HIDDEN);
                    lv_obj_fade_in(entry, 300, i * 50);
                } else {
                    lv_obj_add_flag(entry, LV_OBJ_FLAG_HIDDEN);
                }
            }
        }
        
        lv_msgbox_close(lv_event_get_current_target(e));
    }, LV_EVENT_VALUE_CHANGED, NULL);
}