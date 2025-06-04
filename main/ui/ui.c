#include "ui.h"
#include "ui_helpers.h"
#include "screens/ui_dashboard.h"
#include "screens/ui_climate.h"
#include "screens/ui_schedule.h"
#include "screens/ui_system.h"
#include "screens/ui_logs.h"
#include <string.h>
#include "esp_log.h"

static const char *TAG = "ui";

static lv_obj_t *screens[SCREEN_COUNT];
static screen_t current_screen = SCREEN_DASHBOARD;

void ui_init(void) {
    ESP_LOGI(TAG, "Initializing UI");
    init_styles();
    screens[SCREEN_DASHBOARD] = ui_dashboard_create();
    for (int i = SCREEN_DASHBOARD + 1; i < SCREEN_COUNT; i++) {
        screens[i] = NULL;
    }
    lv_scr_load(screens[SCREEN_DASHBOARD]);
}

void ui_update(void) {
    lv_timer_handler();
}

void ui_switch_screen(screen_t screen) {
    if (screen >= SCREEN_COUNT || screen == current_screen) {
        return;
    }
    if (!screens[screen]) {
        switch (screen) {
            case SCREEN_CLIMATE:
                screens[screen] = ui_climate_create();
                break;
            case SCREEN_SCHEDULE:
                screens[screen] = ui_schedule_create();
                break;
            case SCREEN_SYSTEM:
                screens[screen] = ui_system_create();
                break;
            case SCREEN_LOGS:
                screens[screen] = ui_logs_create();
                break;
            default:
                break;
        }
    }
    if (screens[screen]) {
        lv_scr_load(screens[screen]);
        current_screen = screen;
    }
}

screen_t ui_get_active_screen(void) {
    return current_screen;
}

void ui_update_sensor_data(float temperature, float humidity, float light) {
    if (screens[SCREEN_DASHBOARD]) {
        ui_dashboard_update_sensors(temperature, humidity, light);
    }
}

void ui_update_system_status(int battery_level, bool heating_on, bool cooling_on,
                             bool humidifier_on, bool lighting_on) {
    if (screens[SCREEN_DASHBOARD]) {
        ui_dashboard_update_status(battery_level, heating_on, cooling_on,
                                   humidifier_on, lighting_on);
    }
    if (screens[SCREEN_SYSTEM]) {
        ui_system_update_status(battery_level, heating_on, cooling_on,
                                humidifier_on, lighting_on);
    }
}

void ui_add_log_entry(const char* message, bool is_alert) {
    if (screens[SCREEN_LOGS]) {
        ui_logs_add_entry(message, is_alert);
    }
}

void ui_show_alert(const char* title, const char* message) {
    static const char *btns[] = {"OK", ""};
    lv_obj_t *mbox = lv_msgbox_create(NULL, title, message, btns, false);
    lv_obj_add_style(mbox, &style_alert_box, 0);
    lv_obj_center(mbox);
}

