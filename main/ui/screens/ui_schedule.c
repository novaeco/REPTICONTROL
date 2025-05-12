#include "ui_schedule.h"
#include "../ui_helpers.h"
#include "esp_log.h"

static const char *TAG = "ui_schedule";

// Calendar and time picker widgets
static lv_obj_t *calendar;
static lv_obj_t *time_roller;
static lv_obj_t *event_list;

// Scheduled event data structure
typedef struct {
    int day;
    int hour;
    int minute;
    float temperature;
    float humidity;
    float light;
} schedule_event_t;

// Array to store scheduled events (simplified for demo)
static schedule_event_t events[10];
static int event_count = 0;

// Forward declarations
static void calendar_event_cb(lv_event_t *e);
static void add_event_cb(lv_event_t *e);
static void delete_event_cb(lv_event_t *e);
static void update_event_list(void);

// Create the schedule screen
lv_obj_t *ui_schedule_create(void) {
    ESP_LOGI(TAG, "Creating schedule screen");
    
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
    lv_label_set_text(title, "Schedule");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, 16, 0);
    
    // Create main content layout with flex columns
    lv_obj_t *content = lv_obj_create(screen);
    lv_obj_set_size(content, LV_HOR_RES - 32, LV_VER_RES - 128);
    lv_obj_align(content, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_set_flex_flow(content, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(content, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_style(content, &style_card, 0);
    
    // Left column - Calendar and time selection
    lv_obj_t *left_col = lv_obj_create(content);
    lv_obj_set_size(left_col, LV_PCT(48), LV_PCT(100));
    lv_obj_set_style_bg_opa(left_col, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(left_col, 0, 0);
    lv_obj_set_flex_flow(left_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(left_col, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(left_col, 16, 0);
    
    // Calendar widget
    lv_obj_t *cal_label = lv_label_create(left_col);
    lv_label_set_text(cal_label, "Select Day:");
    lv_obj_add_style(cal_label, &style_title, 0);
    
    calendar = lv_calendar_create(left_col);
    lv_obj_set_size(calendar, LV_PCT(100), 200);
    lv_obj_add_event_cb(calendar, calendar_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Time selection
    lv_obj_t *time_label = lv_label_create(left_col);
    lv_label_set_text(time_label, "Select Time:");
    lv_obj_add_style(time_label, &style_title, 0);
    
    // Create time roller for hour:minute selection
    time_roller = lv_roller_create(left_col);
    
    // Create options string for 24-hour format hours
    static char time_options[24 * 4 * 8]; // 24 hours, every 15 minutes (4 per hour), ~8 chars each
    char *p = time_options;
    for (int h = 0; h < 24; h++) {
        for (int m = 0; m < 60; m += 15) {
            p += sprintf(p, "%02d:%02d\n", h, m);
        }
    }
    *(p-1) = '\0'; // remove last \n
    
    lv_roller_set_options(time_roller, time_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(time_roller, 4);
    lv_obj_set_width(time_roller, LV_PCT(80));
    
    // Add add/remove event buttons
    lv_obj_t *btn_row = lv_obj_create(left_col);
    lv_obj_set_size(btn_row, LV_PCT(100), 50);
    lv_obj_set_flex_flow(btn_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_row, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(btn_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btn_row, 0, 0);
    
    lv_obj_t *add_btn = lv_btn_create(btn_row);
    lv_obj_set_size(add_btn, 100, 40);
    lv_obj_add_style(add_btn, &style_btn, 0);
    lv_obj_add_event_cb(add_btn, add_event_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *add_btn_label = lv_label_create(add_btn);
    lv_label_set_text(add_btn_label, "Add Event");
    lv_obj_center(add_btn_label);
    
    lv_obj_t *del_btn = lv_btn_create(btn_row);
    lv_obj_set_size(del_btn, 100, 40);
    lv_obj_set_style_bg_color(del_btn, COLOR_ERROR, 0);
    lv_obj_add_event_cb(del_btn, delete_event_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *del_btn_label = lv_label_create(del_btn);
    lv_label_set_text(del_btn_label, "Delete");
    lv_obj_center(del_btn_label);
    
    // Right column - Event list
    lv_obj_t *right_col = lv_obj_create(content);
    lv_obj_set_size(right_col, LV_PCT(48), LV_PCT(100));
    lv_obj_set_style_bg_opa(right_col, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(right_col, 0, 0);
    lv_obj_set_flex_flow(right_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(right_col, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(right_col, 16, 0);
    
    // Event list title
    lv_obj_t *event_title = lv_label_create(right_col);
    lv_label_set_text(event_title, "Scheduled Events:");
    lv_obj_add_style(event_title, &style_title, 0);
    
    // Event list
    event_list = lv_list_create(right_col);
    lv_obj_set_size(event_list, LV_PCT(100), LV_PCT(90));
    lv_obj_set_style_bg_color(event_list, lv_color_white(), 0);
    
    // Create sample events for demo
    ui_schedule_add_event(1, 8, 0, 25.0, 60.0, 80.0);
    ui_schedule_add_event(1, 20, 0, 22.0, 70.0, 10.0);
    ui_schedule_add_event(3, 10, 0, 28.0, 50.0, 90.0);
    
    return screen;
}

// Add a scheduled event to the calendar
void ui_schedule_add_event(int day, int hour, int minute, 
                        float temperature, float humidity, float light) {
    if (event_count >= 10) {
        ESP_LOGE(TAG, "Max event limit reached");
        return;
    }
    
    // Store event
    events[event_count].day = day;
    events[event_count].hour = hour;
    events[event_count].minute = minute;
    events[event_count].temperature = temperature;
    events[event_count].humidity = humidity;
    events[event_count].light = light;
    
    event_count++;
    
    // Update the UI list
    update_event_list();
}

// Remove a scheduled event from the calendar
void ui_schedule_remove_event(int day, int hour, int minute) {
    // Find and remove event
    for (int i = 0; i < event_count; i++) {
        if (events[i].day == day && events[i].hour == hour && events[i].minute == minute) {
            // Shift remaining events
            for (int j = i; j < event_count - 1; j++) {
                events[j] = events[j + 1];
            }
            event_count--;
            break;
        }
    }
    
    // Update the UI list
    update_event_list();
}

// Calendar event callback
static void calendar_event_cb(lv_event_t *e) {
    // Update date selection (this would be used when adding events)
}

// Add event button callback
static void add_event_cb(lv_event_t *e) {
    // Get selected date
    lv_calendar_date_t date;
    lv_calendar_get_pressed_date(calendar, &date);
    
    // Get selected time
    char time_str[8];
    lv_roller_get_selected_str(time_roller, time_str, sizeof(time_str));
    
    // Parse time string
    int hour, minute;
    sscanf(time_str, "%d:%d", &hour, &minute);
    
    // For this demo, use default values
    float temp = 25.0f;
    float humidity = 60.0f;
    float light = 75.0f;
    
    // Add the event
    ui_schedule_add_event(date.day, hour, minute, temp, humidity, light);
}

// Delete event button callback
static void delete_event_cb(lv_event_t *e) {
    // In a complete implementation, this would delete the selected event
    // For simplicity, we'll just remove the last event if any exist
    if (event_count > 0) {
        event_count--;
        update_event_list();
    }
}

// Update the event list display
static void update_event_list(void) {
    // Clear the list
    lv_obj_clean(event_list);
    
    // Add day names for reference
    static const char *day_names[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    
    // Repopulate with current events
    for (int i = 0; i < event_count; i++) {
        schedule_event_t *evt = &events[i];
        
        // Create list item
        lv_obj_t *item = lv_list_add_btn(event_list, LV_SYMBOL_CALENDAR, NULL);
        lv_obj_set_style_bg_color(item, lv_color_lighten(COLOR_PRIMARY, 200), 0);
        lv_obj_set_style_bg_opa(item, LV_OPA_20, 0);
        
        // Create a layout for the event details
        lv_obj_t *layout = lv_obj_create(item);
        lv_obj_set_size(layout, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_style_bg_opa(layout, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(layout, 0, 0);
        lv_obj_set_style_pad_all(layout, 0, 0);
        
        // Time and day
        lv_obj_t *time_label = lv_label_create(layout);
        lv_label_set_text_fmt(time_label, "%s %02d:%02d", 
                            day_names[evt->day-1], evt->hour, evt->minute);
        lv_obj_set_style_text_font(time_label, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(time_label, COLOR_PRIMARY, 0);
        lv_obj_align(time_label, LV_ALIGN_TOP_LEFT, 5, 5);
        
        // Settings summary
        lv_obj_t *settings_label = lv_label_create(layout);
        lv_label_set_text_fmt(settings_label, "T: %.1f°C  H: %.1f%%  L: %.1f%%",
                            evt->temperature, evt->humidity, evt->light);
        lv_obj_set_style_text_font(settings_label, &lv_font_montserrat_12, 0);
        lv_obj_align(settings_label, LV_ALIGN_TOP_LEFT, 5, 25);
    }
}