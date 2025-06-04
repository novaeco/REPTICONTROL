#include "ui_dashboard.h"
#include "../ui_helpers.h"
#include "esp_log.h"

static const char *TAG = "ui_dashboard";

// Store gauge objects for updating
static lv_obj_t *gauge_temp;
static lv_obj_t *gauge_humidity;
static lv_obj_t *gauge_light;
static lv_obj_t *value_temp;
static lv_obj_t *value_humidity;
static lv_obj_t *value_light;
static lv_obj_t *time_label;
static lv_obj_t *date_label;
static lv_obj_t *status_panel;
static lv_obj_t *status_icons[4]; // heating, cooling, humidifier, lighting
static lv_obj_t *battery_label;
static lv_obj_t *reptile_img;

// Create the dashboard screen
lv_obj_t *ui_dashboard_create(void) {
    ESP_LOGI(TAG, "Creating dashboard screen");

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

    // Add ReptiControl logo/title to header
    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, "ReptiControl");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, GRID_UNIT * 2, 0);

    // Add clock to header
    time_label = lv_label_create(header);
    lv_label_set_text(time_label, "12:00");
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_16, 0);
    lv_obj_align(time_label, LV_ALIGN_RIGHT_MID, -GRID_UNIT * 2, 0);

    // Add date to header
    date_label = lv_label_create(header);
    lv_label_set_text(date_label, "Mon, Jan 1");
    lv_obj_set_style_text_font(date_label, &lv_font_montserrat_12, 0);
    lv_obj_align(date_label, LV_ALIGN_RIGHT_MID, -GRID_UNIT * 10, 0);

    // Create main content container with grid layout
    lv_obj_t *content = lv_obj_create(screen);
    lv_obj_set_size(content, LV_HOR_RES - SCREEN_PADDING * 2, LV_VER_RES - TOUCH_TARGET_MIN - SCREEN_PADDING * 2);
    lv_obj_align(content, LV_ALIGN_TOP_MID, 0, TOUCH_TARGET_MIN + GRID_UNIT);
    lv_obj_set_grid_dsc_array(content, (lv_coord_t[]){LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST},
                             (lv_coord_t[]){LV_GRID_FR(2), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST});
    lv_obj_set_style_pad_gap(content, GRID_UNIT * 2, 0);

    // Create sensor readings card
    lv_obj_t *sensors_card = create_card(content, "Environmental Readings", LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(sensors_card, 0, 2, 1, LV_GRID_ALIGN_STRETCH, LV_GRID_ALIGN_STRETCH);

    // Create gauge container with flex layout
    lv_obj_t *gauge_container = lv_obj_create(sensors_card);
    lv_obj_set_size(gauge_container, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(gauge_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(gauge_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_top(gauge_container, GRID_UNIT * 4, 0);

    // Create gauges
    gauge_temp = create_gauge(gauge_container, "Temperature", "°C", 15, 40, COLOR_ERROR);
    gauge_humidity = create_gauge(gauge_container, "Humidity", "%", 0, 100, COLOR_INFO);
    gauge_light = create_gauge(gauge_container, "Light", "%", 0, 100, COLOR_WARNING);

    // Create status card
    lv_obj_t *status_card = create_card(content, "System Status", LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(status_card, 1, 1, 1, LV_GRID_ALIGN_STRETCH, LV_GRID_ALIGN_STRETCH);

    // Create status indicators
    const char *status_names[4] = {"Heating", "Cooling", "Humidifier", "Light"};
    const lv_color_t status_colors[4] = {COLOR_ERROR, COLOR_INFO, COLOR_INFO, COLOR_WARNING};

    for (int i = 0; i < 4; i++) {
        lv_obj_t *status_row = lv_obj_create(status_card);
        lv_obj_set_size(status_row, LV_PCT(100), TOUCH_TARGET_MIN);
        lv_obj_set_style_pad_all(status_row, GRID_UNIT, 0);
        lv_obj_set_flex_flow(status_row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(status_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        lv_obj_t *label = lv_label_create(status_row);
        lv_label_set_text(label, status_names[i]);

        status_icons[i] = create_badge(status_row, "OFF", status_colors[i]);
    }

    // Create reptile activity card
    lv_obj_t *activity_card = create_card(content, "Reptile Activity", LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(activity_card, 1, 2, 1, LV_GRID_ALIGN_STRETCH, LV_GRID_ALIGN_STRETCH);

    // Add reptile visualization
    reptile_img = lv_label_create(activity_card);
    lv_label_set_text(reptile_img, LV_SYMBOL_HOME);
    lv_obj_set_style_text_font(reptile_img, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(reptile_img, COLOR_PRIMARY, 0);
    lv_obj_center(reptile_img);

    // Add activity level indicator
    lv_obj_t *activity_label = lv_label_create(activity_card);
    lv_label_set_text(activity_label, "Activity Level: Normal");
    lv_obj_add_style(activity_label, &style_text_muted, 0);
    lv_obj_align(activity_label, LV_ALIGN_BOTTOM_MID, 0, -GRID_UNIT);

    return screen;
}

// Update dashboard with sensor data
void ui_dashboard_update_sensors(float temperature, float humidity, float light) {
    // Update gauges with smooth animations
    animate_gauge_value(gauge_temp, lv_meter_get_indicator_value(gauge_temp, 0), (int)temperature, TRANSITION_TIME);
    animate_gauge_value(gauge_humidity, lv_meter_get_indicator_value(gauge_humidity, 0), (int)humidity, TRANSITION_TIME);
    animate_gauge_value(gauge_light, lv_meter_get_indicator_value(gauge_light, 0), (int)light, TRANSITION_TIME);

    // Update value labels
    lv_label_set_text_fmt(value_temp, "%.1f °C", temperature);
    lv_label_set_text_fmt(value_humidity, "%.1f %%", humidity);
    lv_label_set_text_fmt(value_light, "%.1f %%", light);

    // Update reptile animation based on conditions
    ui_dashboard_update_reptile_animation(temperature, humidity, light);
}

// Update dashboard with system status
void ui_dashboard_update_status(int battery_level, bool heating_on, bool cooling_on,
                              bool humidifier_on, bool lighting_on) {
    // Update status badges
    lv_label_set_text(lv_obj_get_child(status_icons[0], 0), heating_on ? "ON" : "OFF");
    lv_label_set_text(lv_obj_get_child(status_icons[1], 0), cooling_on ? "ON" : "OFF");
    lv_label_set_text(lv_obj_get_child(status_icons[2], 0), humidifier_on ? "ON" : "OFF");
    lv_label_set_text(lv_obj_get_child(status_icons[3], 0), lighting_on ? "ON" : "OFF");

    // Add pulse animation to active systems
    if (heating_on) animate_pulse(status_icons[0], 1500);
    if (cooling_on) animate_pulse(status_icons[1], 1500);
    if (humidifier_on) animate_pulse(status_icons[2], 1500);
    if (lighting_on) animate_pulse(status_icons[3], 1500);

    // Update battery indicator with appropriate icon and color
    const char *batt_icon;
    lv_color_t batt_color;

    if (battery_level > 80) {
        batt_icon = LV_SYMBOL_BATTERY_FULL;
        batt_color = COLOR_SUCCESS;
    } else if (battery_level > 60) {
        batt_icon = LV_SYMBOL_BATTERY_3;
        batt_color = COLOR_SUCCESS;
    } else if (battery_level > 40) {
        batt_icon = LV_SYMBOL_BATTERY_2;
        batt_color = COLOR_WARNING;
    } else if (battery_level > 20) {
        batt_icon = LV_SYMBOL_BATTERY_1;
        batt_color = COLOR_WARNING;
    } else {
        batt_icon = LV_SYMBOL_BATTERY_EMPTY;
        batt_color = COLOR_ERROR;
    }

    if (battery_label) {
        lv_label_set_text_fmt(battery_label, "%s %d%%", batt_icon, battery_level);
        lv_obj_set_style_text_color(battery_label, batt_color, 0);
    }
}

// Update time display
void ui_dashboard_update_time(int hour, int minute) {
    lv_label_set_text_fmt(time_label, "%02d:%02d", hour, minute);
}

// Update reptile animation based on conditions
void ui_dashboard_update_reptile_animation(float temperature, float humidity, float light) {
    const char *activity_icon;
    lv_color_t activity_color;

    if (temperature > 30) {
        activity_icon = LV_SYMBOL_PLUS;
        activity_color = COLOR_ERROR;
    } else if (temperature < 20) {
        activity_icon = LV_SYMBOL_MINUS;
        activity_color = COLOR_INFO;
    } else {
        activity_icon = LV_SYMBOL_OK;
        activity_color = COLOR_SUCCESS;
    }

    lv_label_set_text(reptile_img, activity_icon);
    lv_obj_set_style_text_color(reptile_img, activity_color, 0);
}
