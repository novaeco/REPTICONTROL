#include "ui.h"
#include "ui_helpers.h"
#include "ui_timer.h"
#include "screens/ui_dashboard.h"
#include "screens/ui_climate.h"
#include "screens/ui_schedule.h"
#include "screens/ui_system.h"
#include "screens/ui_logs.h"
#include "drivers/display_driver.h"
#include "drivers/touch_driver.h"
#include "utils/animations.h"
#include "esp_log.h"

static const char *TAG = "ui";

// Screen objects
static lv_obj_t *screens[SCREEN_COUNT];
static screen_t active_screen = SCREEN_DASHBOARD;

// Navigation bar
static lv_obj_t *nav_bar;
static lv_obj_t *nav_buttons[SCREEN_COUNT];

// Screen switching animation
static lv_anim_t screen_switch_anim;

// Forward declarations of internal functions
static void create_navigation_bar(void);
static void nav_button_event_cb(lv_event_t *e);

void ui_init(void) {
    ESP_LOGI(TAG, "Initializing UI");
    
    // Initialize LVGL timer
    ESP_ERROR_CHECK(ui_timer_init());
    
    // Set theme
    lv_theme_t *theme = lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_GREEN),
                                            lv_palette_main(LV_PALETTE_GREY),
                                            true, LV_FONT_DEFAULT);
    lv_disp_set_theme(NULL, theme);
    
    // Initialize styles
    init_styles();
    
    // Create screens
    screens[SCREEN_DASHBOARD] = ui_dashboard_create();
    screens[SCREEN_CLIMATE] = ui_climate_create();
    screens[SCREEN_SCHEDULE] = ui_schedule_create();
    screens[SCREEN_SYSTEM] = ui_system_create();
    screens[SCREEN_LOGS] = ui_logs_create();
    
    // Create navigation bar
    create_navigation_bar();
    
    // Set up screen switch animation parameters
    lv_anim_init(&screen_switch_anim);
    lv_anim_set_time(&screen_switch_anim, 300);
    lv_anim_set_path_cb(&screen_switch_anim, lv_anim_path_ease_out);
    
    // Initial screen load
    lv_scr_load(screens[SCREEN_DASHBOARD]);
    
    ESP_LOGI(TAG, "UI initialized successfully");
}

void ui_update(void) {
    // Let LVGL do its work
    lv_timer_handler();
}

void ui_switch_screen(screen_t screen) {
    if (screen >= SCREEN_COUNT || screen == active_screen) {
        return;
    }
    
    // Select direction for animation (left or right)
    bool slide_left = screen > active_screen;
    
    // Set up source and target screens
    lv_obj_t *scr_from = lv_scr_act();
    lv_obj_t *scr_to = screens[screen];
    
    // Prepare target screen for animation
    lv_obj_set_style_opa(scr_to, LV_OPA_TRANSP, 0);
    
    // Execute the screen change with animation
    animate_screen_change(scr_from, scr_to, slide_left);
    
    // Update active screen
    active_screen = screen;
    
    // Update nav button states
    for (int i = 0; i < SCREEN_COUNT; i++) {
        lv_obj_t *btn = nav_buttons[i];
        if (i == active_screen) {
            lv_obj_add_state(btn, LV_STATE_CHECKED);
        } else {
            lv_obj_clear_state(btn, LV_STATE_CHECKED);
        }
    }
}

screen_t ui_get_active_screen(void) {
    return active_screen;
}

void ui_update_sensor_data(float temperature, float humidity, float light) {
    // Update dashboard screen with new sensor data
    ui_dashboard_update_sensors(temperature, humidity, light);
}

void ui_update_system_status(int battery_level, bool heating_on, bool cooling_on, 
                           bool humidifier_on, bool lighting_on) {
    // Update system status indicators on dashboard and system screens
    ui_dashboard_update_status(battery_level, heating_on, cooling_on, humidifier_on, lighting_on);
    ui_system_update_status(battery_level, heating_on, cooling_on, humidifier_on, lighting_on);
}

void ui_add_log_entry(const char* message, bool is_alert) {
    // Add entry to log screen
    ui_logs_add_entry(message, is_alert);
}

void ui_show_alert(const char* title, const char* message) {
    // Create a message box
    static const char* btns[] = {"OK", ""};
    
    lv_obj_t *mbox = lv_msgbox_create(NULL, title, message, btns, false);
    lv_obj_add_style(mbox, &style_alert_box, 0);
    lv_obj_align(mbox, LV_ALIGN_CENTER, 0, 0);
    
    // Add a close event
    lv_obj_add_event_cb(mbox, [](lv_event_t *e) {
        lv_obj_t *obj = lv_event_get_current_target(e);
        lv_msgbox_close(obj);
    }, LV_EVENT_VALUE_CHANGED, NULL);
}

// Create the navigation bar at the bottom of the screen
static void create_navigation_bar(void) {
    nav_bar = lv_obj_create(lv_scr_act());
    lv_obj_set_size(nav_bar, LV_HOR_RES, NAV_BAR_HEIGHT);
    lv_obj_align(nav_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_flex_flow(nav_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(nav_bar, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_color(nav_bar, COLOR_PRIMARY, 0);
    lv_obj_set_style_radius(nav_bar, 0, 0);
    lv_obj_clear_flag(nav_bar, LV_OBJ_FLAG_SCROLLABLE);
    
    // Navigation items configuration
    static const struct {
        const char *icon;
        const char *text;
    } nav_items[SCREEN_COUNT] = {
        {LV_SYMBOL_HOME, "Home"},
        {LV_SYMBOL_SETTINGS, "Climate"},
        {LV_SYMBOL_CALENDAR, "Schedule"},
        {LV_SYMBOL_WARNING, "System"},
        {LV_SYMBOL_LIST, "Logs"}
    };
    
    // Create navigation items
    for (int i = 0; i < SCREEN_COUNT; i++) {
        nav_buttons[i] = create_nav_item(nav_bar, 
                                       nav_items[i].icon,
                                       nav_items[i].text,
                                       i == 0,  // First item active
                                       nav_button_event_cb);
        
        // Store index for event handling
        lv_obj_set_user_data(nav_buttons[i], (void*)(intptr_t)i);
    }
}

// Handle navigation button clicks
static void nav_button_event_cb(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    int btn_idx = (int)(intptr_t)lv_obj_get_user_data(btn);
    
    // Update active states
    for (int i = 0; i < SCREEN_COUNT; i++) {
        if (i == btn_idx) {
            lv_obj_add_state(nav_buttons[i], LV_STATE_CHECKED);
        } else {
            lv_obj_clear_state(nav_buttons[i], LV_STATE_CHECKED);
        }
    }
    
    // Switch screen with animation
    ui_switch_screen((screen_t)btn_idx);
}