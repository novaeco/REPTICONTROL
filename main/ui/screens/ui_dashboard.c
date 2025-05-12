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
    lv_obj_set_style_bg_color(screen, lv_color_hex(0xF5F5F5), 0); // Light gray background
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    
    // Create header panel
    lv_obj_t *header = lv_obj_create(screen);
    lv_obj_set_size(header, LV_HOR_RES, 48);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_add_style(header, &style_header, 0);
    lv_obj_clear_flag(header, LV_OBJ_FLAG_SCROLLABLE);
    
    // Add ReptiControl logo/title to header
    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, "ReptiControl");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, 16, 0);
    
    // Add clock to header
    time_label = lv_label_create(header);
    lv_label_set_text(time_label, "12:00");
    lv_obj_set_style_text_font(time_label, &lv_font_montserrat_16, 0);
    lv_obj_align(time_label, LV_ALIGN_RIGHT_MID, -16, 0);
    
    // Add date to header
    date_label = lv_label_create(header);
    lv_label_set_text(date_label, "Mon, Jan 1");
    lv_obj_set_style_text_font(date_label, &lv_font_montserrat_12, 0);
    lv_obj_align(date_label, LV_ALIGN_RIGHT_MID, -80, 0);
    
    // Create gauges panel
    lv_obj_t *gauges_panel = lv_obj_create(screen);
    lv_obj_set_size(gauges_panel, LV_HOR_RES - 32, 200);
    lv_obj_align(gauges_panel, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_set_flex_flow(gauges_panel, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(gauges_panel, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_style(gauges_panel, &style_card, 0);
    lv_obj_clear_flag(gauges_panel, LV_OBJ_FLAG_SCROLLABLE);
    
    // Create temperature gauge
    lv_obj_t *temp_panel = lv_obj_create(gauges_panel);
    lv_obj_set_size(temp_panel, 140, 180);
    lv_obj_set_style_bg_opa(temp_panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(temp_panel, 0, 0);
    lv_obj_clear_flag(temp_panel, LV_OBJ_FLAG_SCROLLABLE);
    
    gauge_temp = lv_meter_create(temp_panel);
    lv_obj_set_size(gauge_temp, 120, 120);
    lv_obj_center(gauge_temp);
    
    // Configure temperature gauge
    lv_meter_scale_t *temp_scale = lv_meter_add_scale(gauge_temp);
    lv_meter_set_scale_ticks(gauge_temp, temp_scale, 41, 2, 10, lv_color_hex(0xE53935)); // Red
    lv_meter_set_scale_range(gauge_temp, temp_scale, 10, 40, 270, 135);
    
    // Add needle indicator
    lv_meter_indicator_t *temp_indic = lv_meter_add_needle_line(gauge_temp, temp_scale, 4, lv_color_hex(0xE53935), -10);
    lv_meter_set_indicator_value(gauge_temp, temp_indic, 25); // Default value
    
    // Add temperature label
    lv_obj_t *temp_label = lv_label_create(temp_panel);
    lv_label_set_text(temp_label, "Temperature");
    lv_obj_align(temp_label, LV_ALIGN_TOP_MID, 0, 0);
    
    value_temp = lv_label_create(temp_panel);
    lv_label_set_text(value_temp, "25 °C");
    lv_obj_align(value_temp, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_style(value_temp, &style_value, 0);
    
    // Create humidity gauge
    lv_obj_t *humid_panel = lv_obj_create(gauges_panel);
    lv_obj_set_size(humid_panel, 140, 180);
    lv_obj_set_style_bg_opa(humid_panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(humid_panel, 0, 0);
    lv_obj_clear_flag(humid_panel, LV_OBJ_FLAG_SCROLLABLE);
    
    gauge_humidity = lv_meter_create(humid_panel);
    lv_obj_set_size(gauge_humidity, 120, 120);
    lv_obj_center(gauge_humidity);
    
    // Configure humidity gauge
    lv_meter_scale_t *humid_scale = lv_meter_add_scale(gauge_humidity);
    lv_meter_set_scale_ticks(gauge_humidity, humid_scale, 41, 2, 10, lv_color_hex(0x2196F3)); // Blue
    lv_meter_set_scale_range(gauge_humidity, humid_scale, 0, 100, 270, 135);
    
    // Add needle indicator
    lv_meter_indicator_t *humid_indic = lv_meter_add_needle_line(gauge_humidity, humid_scale, 4, lv_color_hex(0x2196F3), -10);
    lv_meter_set_indicator_value(gauge_humidity, humid_indic, 50); // Default value
    
    // Add humidity label
    lv_obj_t *humid_label = lv_label_create(humid_panel);
    lv_label_set_text(humid_label, "Humidity");
    lv_obj_align(humid_label, LV_ALIGN_TOP_MID, 0, 0);
    
    value_humidity = lv_label_create(humid_panel);
    lv_label_set_text(value_humidity, "50 %");
    lv_obj_align(value_humidity, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_style(value_humidity, &style_value, 0);
    
    // Create light gauge
    lv_obj_t *light_panel = lv_obj_create(gauges_panel);
    lv_obj_set_size(light_panel, 140, 180);
    lv_obj_set_style_bg_opa(light_panel, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(light_panel, 0, 0);
    lv_obj_clear_flag(light_panel, LV_OBJ_FLAG_SCROLLABLE);
    
    gauge_light = lv_meter_create(light_panel);
    lv_obj_set_size(gauge_light, 120, 120);
    lv_obj_center(gauge_light);
    
    // Configure light gauge
    lv_meter_scale_t *light_scale = lv_meter_add_scale(gauge_light);
    lv_meter_set_scale_ticks(gauge_light, light_scale, 41, 2, 10, lv_color_hex(0xFFB300)); // Amber
    lv_meter_set_scale_range(gauge_light, light_scale, 0, 100, 270, 135);
    
    // Add needle indicator
    lv_meter_indicator_t *light_indic = lv_meter_add_needle_line(gauge_light, light_scale, 4, lv_color_hex(0xFFB300), -10);
    lv_meter_set_indicator_value(gauge_light, light_indic, 75); // Default value
    
    // Add light label
    lv_obj_t *light_label = lv_label_create(light_panel);
    lv_label_set_text(light_label, "Light");
    lv_obj_align(light_label, LV_ALIGN_TOP_MID, 0, 0);
    
    value_light = lv_label_create(light_panel);
    lv_label_set_text(value_light, "75 %");
    lv_obj_align(value_light, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_add_style(value_light, &style_value, 0);
    
    // Create status panel with device status indicators
    status_panel = lv_obj_create(screen);
    lv_obj_set_size(status_panel, LV_HOR_RES / 2 - 24, 120);
    lv_obj_align(status_panel, LV_ALIGN_TOP_LEFT, 16, 280);
    lv_obj_add_style(status_panel, &style_card, 0);
    lv_obj_clear_flag(status_panel, LV_OBJ_FLAG_SCROLLABLE);
    
    // Add status title
    lv_obj_t *status_title = lv_label_create(status_panel);
    lv_label_set_text(status_title, "System Status");
    lv_obj_add_style(status_title, &style_title, 0);
    lv_obj_align(status_title, LV_ALIGN_TOP_MID, 0, 0);
    
    // Create status indicators
    const char *status_names[4] = {"Heating", "Cooling", "Humid", "Light"};
    const char *status_icons_text[4] = {LV_SYMBOL_DIRECTORY, LV_SYMBOL_REFRESH, LV_SYMBOL_UPLOAD, LV_SYMBOL_DRIVE};
    const lv_color_t status_colors[4] = {
        lv_color_hex(0xE53935), // Red for heating
        lv_color_hex(0x2196F3), // Blue for cooling
        lv_color_hex(0x00ACC1), // Cyan for humidifier
        lv_color_hex(0xFFB300)  // Amber for lighting
    };
    
    // Create grid layout for status indicators
    lv_obj_t *status_grid = lv_obj_create(status_panel);
    lv_obj_set_size(status_grid, LV_PCT(100), 70);
    lv_obj_align(status_grid, LV_ALIGN_BOTTOM_MID, 0, -8);
    lv_obj_set_style_bg_opa(status_grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(status_grid, 0, 0);
    lv_obj_set_grid_dsc_array(status_grid, (lv_coord_t[]){LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST}, 
                             (lv_coord_t[]){LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST});
    
    // Add individual status indicators
    for (int i = 0; i < 4; i++) {
        int col = i % 2;
        int row = i / 2;
        
        // Create container for each status
        lv_obj_t *status_cont = lv_obj_create(status_grid);
        lv_obj_set_grid_cell(status_cont, LV_GRID_ALIGN_CENTER, col, 1, LV_GRID_ALIGN_CENTER, row, 1);
        lv_obj_set_size(status_cont, LV_PCT(90), LV_PCT(80));
        lv_obj_set_style_bg_opa(status_cont, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(status_cont, 0, 0);
        
        // Add icon
        status_icons[i] = lv_label_create(status_cont);
        lv_label_set_text(status_icons[i], status_icons_text[i]);
        lv_obj_set_style_text_color(status_icons[i], status_colors[i], 0);
        lv_obj_set_style_text_color(status_icons[i], lv_color_hex(0xBDBDBD), LV_STATE_DISABLED);
        lv_obj_align(status_icons[i], LV_ALIGN_TOP_MID, 0, 0);
        lv_obj_add_state(status_icons[i], LV_STATE_DISABLED); // Start disabled
        
        // Add label
        lv_obj_t *status_label = lv_label_create(status_cont);
        lv_label_set_text(status_label, status_names[i]);
        lv_obj_set_style_text_font(status_label, &lv_font_montserrat_12, 0);
        lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, 0);
    }
    
    // Add battery indicator
    battery_label = lv_label_create(status_panel);
    lv_label_set_text(battery_label, LV_SYMBOL_BATTERY_FULL " 100%");
    lv_obj_align(battery_label, LV_ALIGN_BOTTOM_RIGHT, -8, -8);
    
    // Create reptile visualization panel
    lv_obj_t *reptile_panel = lv_obj_create(screen);
    lv_obj_set_size(reptile_panel, LV_HOR_RES / 2 - 24, 120);
    lv_obj_align(reptile_panel, LV_ALIGN_TOP_RIGHT, -16, 280);
    lv_obj_add_style(reptile_panel, &style_card, 0);
    lv_obj_clear_flag(reptile_panel, LV_OBJ_FLAG_SCROLLABLE);
    
    // Add title
    lv_obj_t *reptile_title = lv_label_create(reptile_panel);
    lv_label_set_text(reptile_title, "Reptile Activity");
    lv_obj_add_style(reptile_title, &style_title, 0);
    lv_obj_align(reptile_title, LV_ALIGN_TOP_MID, 0, 0);
    
    // Simple reptile visualization (placeholder for animation)
    reptile_img = lv_label_create(reptile_panel);
    lv_label_set_text(reptile_img, LV_SYMBOL_HOME);  // Placeholder
    lv_obj_set_style_text_font(reptile_img, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(reptile_img, COLOR_PRIMARY, 0);
    lv_obj_center(reptile_img);
    
    return screen;
}

// Update dashboard with sensor data
void ui_dashboard_update_sensors(float temperature, float humidity, float light) {
    // Update gauges
    lv_meter_set_indicator_value(gauge_temp, lv_meter_get_indicator(gauge_temp, 0), (int)temperature);
    lv_meter_set_indicator_value(gauge_humidity, lv_meter_get_indicator(gauge_humidity, 0), (int)humidity);
    lv_meter_set_indicator_value(gauge_light, lv_meter_get_indicator(gauge_light, 0), (int)light);
    
    // Update value labels
    lv_label_set_text_fmt(value_temp, "%.1f °C", temperature);
    lv_label_set_text_fmt(value_humidity, "%.1f %%", humidity);
    lv_label_set_text_fmt(value_light, "%.1f %%", light);
    
    // Update reptile animation based on current conditions
    ui_dashboard_update_reptile_animation(temperature, humidity, light);
}

// Update dashboard with system status
void ui_dashboard_update_status(int battery_level, bool heating_on, bool cooling_on, 
                              bool humidifier_on, bool lighting_on) {
    // Update status indicators
    if (heating_on) {
        lv_obj_clear_state(status_icons[0], LV_STATE_DISABLED);
    } else {
        lv_obj_add_state(status_icons[0], LV_STATE_DISABLED);
    }
    
    if (cooling_on) {
        lv_obj_clear_state(status_icons[1], LV_STATE_DISABLED);
    } else {
        lv_obj_add_state(status_icons[1], LV_STATE_DISABLED);
    }
    
    if (humidifier_on) {
        lv_obj_clear_state(status_icons[2], LV_STATE_DISABLED);
    } else {
        lv_obj_add_state(status_icons[2], LV_STATE_DISABLED);
    }
    
    if (lighting_on) {
        lv_obj_clear_state(status_icons[3], LV_STATE_DISABLED);
    } else {
        lv_obj_add_state(status_icons[3], LV_STATE_DISABLED);
    }
    
    // Update battery indicator with appropriate icon
    const char *batt_icon;
    if (battery_level > 80) {
        batt_icon = LV_SYMBOL_BATTERY_FULL;
    } else if (battery_level > 60) {
        batt_icon = LV_SYMBOL_BATTERY_3;
    } else if (battery_level > 40) {
        batt_icon = LV_SYMBOL_BATTERY_2;
    } else if (battery_level > 20) {
        batt_icon = LV_SYMBOL_BATTERY_1;
    } else {
        batt_icon = LV_SYMBOL_BATTERY_EMPTY;
    }
    
    lv_label_set_text_fmt(battery_label, "%s %d%%", batt_icon, battery_level);
}

// Update time display
void ui_dashboard_update_time(int hour, int minute) {
    lv_label_set_text_fmt(time_label, "%02d:%02d", hour, minute);
}

// Update reptile animation based on conditions
void ui_dashboard_update_reptile_animation(float temperature, float humidity, float light) {
    // Simplified animation based on temperature
    // In a real implementation, this would be more sophisticated
    if (temperature > 30) {
        // Hot - basking reptile
        lv_label_set_text(reptile_img, LV_SYMBOL_PLUS);
        lv_obj_set_style_text_color(reptile_img, lv_color_hex(0xE53935), 0); // Red
    } else if (temperature < 20) {
        // Cold - hiding reptile
        lv_label_set_text(reptile_img, LV_SYMBOL_MINUS);
        lv_obj_set_style_text_color(reptile_img, lv_color_hex(0x2196F3), 0); // Blue
    } else {
        // Optimal - active reptile
        lv_label_set_text(reptile_img, LV_SYMBOL_OK);
        lv_obj_set_style_text_color(reptile_img, COLOR_SUCCESS, 0); // Green
    }
}