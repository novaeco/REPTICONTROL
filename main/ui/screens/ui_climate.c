#include "ui_climate.h"
#include "../ui_helpers.h"
#include "esp_log.h"

static const char *TAG = "ui_climate";

// Store slider and toggle references for updating
static lv_obj_t *slider_temp;
static lv_obj_t *slider_humidity;
static lv_obj_t *slider_light;
static lv_obj_t *temp_value_label;
static lv_obj_t *humidity_value_label;
static lv_obj_t *light_value_label;
static lv_obj_t *toggle_heating;
static lv_obj_t *toggle_cooling;
static lv_obj_t *toggle_humidifier;
static lv_obj_t *toggle_lighting;

// Callback prototypes
static void temp_slider_cb(lv_event_t *e);
static void humidity_slider_cb(lv_event_t *e);
static void light_slider_cb(lv_event_t *e);
static void toggle_cb(lv_event_t *e);

// Create the climate control screen
lv_obj_t *ui_climate_create(void) {
    ESP_LOGI(TAG, "Creating climate screen");
    
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
    lv_label_set_text(title, "Climate Control");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, 16, 0);
    
    // Main content container
    lv_obj_t *content = lv_obj_create(screen);
    lv_obj_set_size(content, LV_HOR_RES - 32, LV_VER_RES - 128);
    lv_obj_align(content, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_add_style(content, &style_card, 0);
    lv_obj_set_flex_flow(content, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(content, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(content, 16, 0);
    
    // Create temperature control
    lv_obj_t *temp_title = lv_label_create(content);
    lv_label_set_text(temp_title, "Temperature");
    lv_obj_add_style(temp_title, &style_title, 0);
    
    // Temperature slider
    lv_obj_t *temp_slider_cont = lv_obj_create(content);
    lv_obj_set_size(temp_slider_cont, LV_PCT(90), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(temp_slider_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(temp_slider_cont, 0, 0);
    
    temp_value_label = lv_label_create(temp_slider_cont);
    lv_label_set_text(temp_value_label, "25°C");
    lv_obj_align(temp_value_label, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_style(temp_value_label, &style_value, 0);
    
    slider_temp = lv_slider_create(temp_slider_cont);
    lv_obj_set_width(slider_temp, LV_PCT(100));
    lv_slider_set_range(slider_temp, 15, 40);  // 15°C to 40°C
    lv_slider_set_value(slider_temp, 25, LV_ANIM_OFF);  // Default: 25°C
    lv_obj_align(slider_temp, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_add_event_cb(slider_temp, temp_slider_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Temperature toggle switches
    lv_obj_t *temp_toggles = lv_obj_create(content);
    lv_obj_set_size(temp_toggles, LV_PCT(90), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(temp_toggles, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(temp_toggles, 0, 0);
    lv_obj_set_flex_flow(temp_toggles, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(temp_toggles, 8, 0);
    
    toggle_heating = create_toggle(temp_toggles, "Heating", true, toggle_cb);
    toggle_cooling = create_toggle(temp_toggles, "Cooling", true, toggle_cb);
    
    // Create humidity control
    lv_obj_t *humidity_title = lv_label_create(content);
    lv_label_set_text(humidity_title, "Humidity");
    lv_obj_add_style(humidity_title, &style_title, 0);
    
    // Humidity slider
    lv_obj_t *humidity_slider_cont = lv_obj_create(content);
    lv_obj_set_size(humidity_slider_cont, LV_PCT(90), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(humidity_slider_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(humidity_slider_cont, 0, 0);
    
    humidity_value_label = lv_label_create(humidity_slider_cont);
    lv_label_set_text(humidity_value_label, "50%");
    lv_obj_align(humidity_value_label, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_style(humidity_value_label, &style_value, 0);
    
    slider_humidity = lv_slider_create(humidity_slider_cont);
    lv_obj_set_width(slider_humidity, LV_PCT(100));
    lv_slider_set_range(slider_humidity, 20, 90);  // 20% to 90%
    lv_slider_set_value(slider_humidity, 50, LV_ANIM_OFF);  // Default: 50%
    lv_obj_align(slider_humidity, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_add_event_cb(slider_humidity, humidity_slider_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Humidity toggle
    lv_obj_t *humidity_toggles = lv_obj_create(content);
    lv_obj_set_size(humidity_toggles, LV_PCT(90), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(humidity_toggles, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(humidity_toggles, 0, 0);
    
    toggle_humidifier = create_toggle(humidity_toggles, "Humidifier", true, toggle_cb);
    
    // Create light control
    lv_obj_t *light_title = lv_label_create(content);
    lv_label_set_text(light_title, "Lighting");
    lv_obj_add_style(light_title, &style_title, 0);
    
    // Light slider
    lv_obj_t *light_slider_cont = lv_obj_create(content);
    lv_obj_set_size(light_slider_cont, LV_PCT(90), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(light_slider_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(light_slider_cont, 0, 0);
    
    light_value_label = lv_label_create(light_slider_cont);
    lv_label_set_text(light_value_label, "75%");
    lv_obj_align(light_value_label, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_style(light_value_label, &style_value, 0);
    
    slider_light = lv_slider_create(light_slider_cont);
    lv_obj_set_width(slider_light, LV_PCT(100));
    lv_slider_set_range(slider_light, 0, 100);  // 0% to 100%
    lv_slider_set_value(slider_light, 75, LV_ANIM_OFF);  // Default: 75%
    lv_obj_align(slider_light, LV_ALIGN_TOP_MID, 0, 40);
    lv_obj_add_event_cb(slider_light, light_slider_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Light toggle
    lv_obj_t *light_toggles = lv_obj_create(content);
    lv_obj_set_size(light_toggles, LV_PCT(90), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(light_toggles, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(light_toggles, 0, 0);
    
    toggle_lighting = create_toggle(light_toggles, "Lighting", true, toggle_cb);
    
    return screen;
}

// Update the climate screen with current values
void ui_climate_update_values(float temp_target, float humidity_target, float light_target,
                           bool heating_on, bool cooling_on, bool humidifier_on, bool lighting_on) {
    // Update sliders
    lv_slider_set_value(slider_temp, (int)temp_target, LV_ANIM_OFF);
    lv_slider_set_value(slider_humidity, (int)humidity_target, LV_ANIM_OFF);
    lv_slider_set_value(slider_light, (int)light_target, LV_ANIM_OFF);
    
    // Update value labels
    lv_label_set_text_fmt(temp_value_label, "%.1f°C", temp_target);
    lv_label_set_text_fmt(humidity_value_label, "%.1f%%", humidity_target);
    lv_label_set_text_fmt(light_value_label, "%.1f%%", light_target);
    
    // Update toggles
    if (heating_on) {
        lv_obj_add_state(toggle_heating, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(toggle_heating, LV_STATE_CHECKED);
    }
    
    if (cooling_on) {
        lv_obj_add_state(toggle_cooling, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(toggle_cooling, LV_STATE_CHECKED);
    }
    
    if (humidifier_on) {
        lv_obj_add_state(toggle_humidifier, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(toggle_humidifier, LV_STATE_CHECKED);
    }
    
    if (lighting_on) {
        lv_obj_add_state(toggle_lighting, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(toggle_lighting, LV_STATE_CHECKED);
    }
}

// Get the current target values from the climate screen
void ui_climate_get_targets(float *temp_target, float *humidity_target, float *light_target,
                          bool *heating_on, bool *cooling_on, bool *humidifier_on, bool *lighting_on) {
    // Get slider values
    *temp_target = (float)lv_slider_get_value(slider_temp);
    *humidity_target = (float)lv_slider_get_value(slider_humidity);
    *light_target = (float)lv_slider_get_value(slider_light);
    
    // Get toggle states
    *heating_on = lv_obj_has_state(toggle_heating, LV_STATE_CHECKED);
    *cooling_on = lv_obj_has_state(toggle_cooling, LV_STATE_CHECKED);
    *humidifier_on = lv_obj_has_state(toggle_humidifier, LV_STATE_CHECKED);
    *lighting_on = lv_obj_has_state(toggle_lighting, LV_STATE_CHECKED);
}

// Slider event callbacks
static void temp_slider_cb(lv_event_t *e) {
    int value = lv_slider_get_value(slider_temp);
    lv_label_set_text_fmt(temp_value_label, "%d°C", value);
}

static void humidity_slider_cb(lv_event_t *e) {
    int value = lv_slider_get_value(slider_humidity);
    lv_label_set_text_fmt(humidity_value_label, "%d%%", value);
}

static void light_slider_cb(lv_event_t *e) {
    int value = lv_slider_get_value(slider_light);
    lv_label_set_text_fmt(light_value_label, "%d%%", value);
}

// Toggle switch callback
static void toggle_cb(lv_event_t *e) {
    // In a complete implementation, this would trigger system actions
    // For the simulator, this is handled in the climate controller
}