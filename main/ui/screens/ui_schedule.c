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
    lv_label_set_text(title, "Schedule");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, GRID_UNIT * 2, 0);
    
    // Create main content container with grid layout
    lv_obj_t *content = lv_obj_create(screen);
    lv_obj_set_size(content, LV_HOR_RES - SCREEN_PADDING * 2, LV_VER_RES - TOUCH_TARGET_MIN - SCREEN_PADDING * 2);
    lv_obj_align(content, LV_ALIGN_TOP_MID, 0, TOUCH_TARGET_MIN + GRID_UNIT);
    lv_obj_set_grid_dsc_array(content, (lv_coord_t[]){LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST},
                             (lv_coord_t[]){LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST});
    lv_obj_set_style_pad_gap(content, GRID_UNIT * 2, 0);
    
    // Left column - Calendar and time selection
    lv_obj_t *left_card = create_card(content, "Add New Event", LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(left_card, 0, 1, 1, LV_GRID_ALIGN_STRETCH, LV_GRID_ALIGN_STRETCH);
    
    // Calendar widget with custom styling
    calendar = lv_calendar_create(left_card);
    lv_obj_set_size(calendar, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_color(calendar, COLOR_SURFACE, 0);
    lv_obj_set_style_border_width(calendar, 1, 0);
    lv_obj_set_style_border_color(calendar, COLOR_SECONDARY, 0);
    lv_obj_set_style_radius(calendar, BORDER_RADIUS, 0);
    lv_obj_set_style_pad_all(calendar, GRID_UNIT, 0);
    lv_obj_align(calendar, LV_ALIGN_TOP_MID, 0, GRID_UNIT * 4);
    lv_obj_add_event_cb(calendar, calendar_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Time selection label
    lv_obj_t *time_label = lv_label_create(left_card);
    lv_label_set_text(time_label, "Select Time:");
    lv_obj_add_style(time_label, &style_title, 0);
    lv_obj_align(time_label, LV_ALIGN_TOP_MID, 0, GRID_UNIT * 30);
    
    // Create time roller with custom styling
    time_roller = lv_roller_create(left_card);
    
    // Create options string for 24-hour format hours
    static char time_options[24 * 4 * 8];
    char *p = time_options;
    for (int h = 0; h < 24; h++) {
        for (int m = 0; m < 60; m += 15) {
            p += sprintf(p, "%02d:%02d\n", h, m);
        }
    }
    *(p-1) = '\0';
    
    lv_roller_set_options(time_roller, time_options, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_visible_row_count(time_roller, 3);
    lv_obj_set_width(time_roller, LV_PCT(80));
    lv_obj_set_style_text_font(time_roller, &lv_font_montserrat_16, 0);
    lv_obj_set_style_bg_color(time_roller, COLOR_SURFACE, 0);
    lv_obj_set_style_border_color(time_roller, COLOR_SECONDARY, 0);
    lv_obj_set_style_radius(time_roller, BORDER_RADIUS, 0);
    lv_obj_align(time_roller, LV_ALIGN_TOP_MID, 0, GRID_UNIT * 35);
    
    // Add action buttons
    lv_obj_t *btn_row = lv_obj_create(left_card);
    lv_obj_set_size(btn_row, LV_PCT(100), TOUCH_TARGET_MIN);
    lv_obj_set_flex_flow(btn_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_row, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(btn_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btn_row, 0, 0);
    lv_obj_align(btn_row, LV_ALIGN_BOTTOM_MID, 0, -GRID_UNIT * 2);
    
    // Add event button
    lv_obj_t *add_btn = lv_btn_create(btn_row);
    lv_obj_set_size(add_btn, TOUCH_TARGET_MIN * 3, TOUCH_TARGET_MIN);
    lv_obj_add_style(add_btn, &style_btn, 0);
    lv_obj_add_event_cb(add_btn, add_event_cb, LV_EVENT_CLICKED, NULL);
    add_ripple_effect(add_btn);
    
    lv_obj_t *add_label = lv_label_create(add_btn);
    lv_label_set_text(add_label, LV_SYMBOL_PLUS " Add Event");
    lv_obj_center(add_label);
    
    // Delete button
    lv_obj_t *del_btn = lv_btn_create(btn_row);
    lv_obj_set_size(del_btn, TOUCH_TARGET_MIN * 3, TOUCH_TARGET_MIN);
    lv_obj_set_style_bg_color(del_btn, COLOR_ERROR, 0);
    lv_obj_add_event_cb(del_btn, delete_event_cb, LV_EVENT_CLICKED, NULL);
    add_ripple_effect(del_btn);
    
    lv_obj_t *del_label = lv_label_create(del_btn);
    lv_label_set_text(del_label, LV_SYMBOL_TRASH " Delete");
    lv_obj_center(del_label);
    
    // Right column - Event list
    lv_obj_t *right_card = create_card(content, "Scheduled Events", LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(right_card, 1, 1, 1, LV_GRID_ALIGN_STRETCH, LV_GRID_ALIGN_STRETCH);
    
    // Create event list with custom styling
    event_list = lv_list_create(right_card);
    lv_obj_set_size(event_list, LV_PCT(100), LV_PCT(90));
    lv_obj_set_style_pad_all(event_list, GRID_UNIT, 0);
    lv_obj_set_style_bg_color(event_list, COLOR_SURFACE, 0);
    lv_obj_set_style_border_color(event_list, COLOR_SECONDARY, 0);
    lv_obj_set_style_radius(event_list, BORDER_RADIUS, 0);
    lv_obj_align(event_list, LV_ALIGN_TOP_MID, 0, GRID_UNIT * 4);
    
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
    lv_obj_t *calendar = lv_event_get_target(e);
    lv_calendar_date_t date;
    lv_calendar_get_pressed_date(calendar, &date);
    
    // Highlight selected date
    static lv_calendar_date_t highlighted_date;
    highlighted_date = date;
    lv_calendar_set_highlighted_dates(calendar, &highlighted_date, 1);
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
    
    // Add the event with animation
    ui_schedule_add_event(date.day, hour, minute, temp, humidity, light);
    
    // Show success message
    static const char *btns[] = {"OK", ""};
    lv_obj_t *mbox = lv_msgbox_create(NULL, "Success", "Event added successfully", btns, false);
    lv_obj_add_style(mbox, &style_alert_box, 0);
    lv_obj_center(mbox);
    
    // Auto-close after delay
    lv_timer_t *timer = lv_timer_create([](lv_timer_t *t) {
        lv_msgbox_close((lv_obj_t *)t->user_data);
    }, 2000, mbox);
    lv_timer_set_repeat_count(timer, 1);
}

// Delete event button callback
static void delete_event_cb(lv_event_t *e) {
    if (event_count > 0) {
        // Show confirmation dialog
        static const char *btns[] = {"Yes", "No", ""};
        lv_obj_t *mbox = lv_msgbox_create(NULL, "Confirm Delete", 
                                         "Delete the selected event?", btns, false);
        lv_obj_add_style(mbox, &style_alert_box, 0);
        lv_obj_center(mbox);
        
        // Add event handler for confirmation
        lv_obj_add_event_cb(mbox, [](lv_event_t *e) {
            const char *btn_text = lv_msgbox_get_active_btn_text(lv_event_get_current_target(e));
            if (strcmp(btn_text, "Yes") == 0) {
                event_count--;
                update_event_list();
            }
            lv_msgbox_close(lv_event_get_current_target(e));
        }, LV_EVENT_VALUE_CHANGED, NULL);
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
        
        // Create list item with custom styling
        lv_obj_t *item = lv_obj_create(event_list);
        lv_obj_set_size(item, LV_PCT(100), TOUCH_TARGET_MIN * 1.5);
        lv_obj_set_style_bg_color(item, lv_color_lighten(COLOR_PRIMARY, 220), 0);
        lv_obj_set_style_bg_opa(item, LV_OPA_20, 0);
        lv_obj_set_style_radius(item, BORDER_RADIUS / 2, 0);
        lv_obj_set_style_pad_all(item, GRID_UNIT, 0);
        add_ripple_effect(item);
        
        // Time badge
        lv_obj_t *time_badge = create_badge(item, "", COLOR_PRIMARY);
        lv_obj_align(time_badge, LV_ALIGN_LEFT_MID, 0, 0);
        lv_label_set_text_fmt(lv_obj_get_child(time_badge, 0), 
                            "%02d:%02d", evt->hour, evt->minute);
        
        // Day label
        lv_obj_t *day_label = lv_label_create(item);
        lv_label_set_text(day_label, day_names[evt->day-1]);
        lv_obj_add_style(day_label, &style_text_muted, 0);
        lv_obj_align(day_label, LV_ALIGN_LEFT_MID, TOUCH_TARGET_MIN * 1.5, 0);
        
        // Settings summary
        lv_obj_t *settings_label = lv_label_create(item);
        lv_label_set_text_fmt(settings_label, "T: %.1f°C  H: %.1f%%  L: %.1f%%",
                            evt->temperature, evt->humidity, evt->light);
        lv_obj_align(settings_label, LV_ALIGN_RIGHT_MID, -GRID_UNIT, 0);
    }
}