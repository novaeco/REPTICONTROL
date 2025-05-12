#include "ui_climate.h"
#include "../ui_helpers.h"
#include "esp_log.h"

static const char *TAG = "ui_climate";

// Store control references
static lv_obj_t *slider_temp;
static lv_obj_t *slider_humidity;
static lv_obj_t *slider_light;
static lv_obj_t *value_temp;
static lv_obj_t *value_humidity;
static lv_obj_t *value_light;
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
    lv_label_set_text(title, "Climate Control");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, GRID_UNIT * 2, 0);
    
    // Create main content container with grid layout
    lv_obj_t *content = lv_obj_create(screen);
    lv_obj_set_size(content, LV_HOR_RES - SCREEN_PADDING * 2, LV_VER_RES - TOUCH_TARGET_MIN - SCREEN_PADDING * 2);
    lv_obj_align(content, LV_ALIGN_TOP_MID, 0, TOUCH_TARGET_MIN + GRID_UNIT);
    lv_obj_set_grid_dsc_array(content, (lv_coord_t[]){LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST},
                             (lv_coord_t[]){LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST});
    lv_obj_set_style_pad_gap(content, GRID_UNIT * 2, 0);
    
    // Temperature control card
    lv_obj_t *temp_card = create_card(content, "Temperature Control", LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(temp_card, 0, 2, 1, LV_GRID_ALIGN_STRETCH, LV_GRID_ALIGN_STRETCH);
    
    // Temperature slider
    slider_temp = create_slider(temp_card, "Target Temperature", 15, 40, 25, temp_slider_cb);
    value_temp = lv_obj_get_child(lv_obj_get_child(slider_temp, 0), 1); // Get value label reference
    
    // Temperature toggles container
    lv_obj_t *temp_toggles = lv_obj_create(temp_card);
    lv_obj_set_size(temp_toggles, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_top(temp_toggles, GRID_UNIT * 3, 0);
    lv_obj_set_style_bg_opa(temp_toggles, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(temp_toggles, 0, 0);
    lv_obj_set_flex_flow(temp_toggles, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(temp_toggles, GRID_UNIT, 0);
    
    toggle_heating = create_toggle(temp_toggles, "Heating System", true, toggle_cb);
    toggle_cooling = create_toggle(temp_toggles, "Cooling System", true, toggle_cb);
    
    // Humidity control card
    lv_obj_t *humidity_card = create_card(content, "Humidity Control", LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(humidity_card, 1, 1, 1, LV_GRID_ALIGN_STRETCH, LV_GRID_ALIGN_STRETCH);
    
    // Humidity slider
    slider_humidity = create_slider(humidity_card, "Target Humidity", 20, 90, 50, humidity_slider_cb);
    value_humidity = lv_obj_get_child(lv_obj_get_child(slider_humidity, 0), 1);
    
    // Humidity toggle
    lv_obj_t *humidity_toggles = lv_obj_create(humidity_card);
    lv_obj_set_size(humidity_toggles, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_top(humidity_toggles, GRID_UNIT * 3, 0);
    lv_obj_set_style_bg_opa(humidity_toggles, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(humidity_toggles, 0, 0);
    
    toggle_humidifier = create_toggle(humidity_toggles, "Humidifier", true, toggle_cb);
    
    // Light control card
    lv_obj_t *light_card = create_card(content, "Lighting Control", LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(light_card, 2, 1, 1, LV_GRID_ALIGN_STRETCH, LV_GRID_ALIGN_STRETCH);
    
    // Light slider
    slider_light = create_slider(light_card, "Target Light Level", 0, 100, 75, light_slider_cb);
    value_light = lv_obj_get_child(lv_obj_get_child(slider_light, 0), 1);
    
    // Light toggle
    lv_obj_t *light_toggles = lv_obj_create(light_card);
    lv_obj_set_size(light_toggles, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_top(light_toggles, GRID_UNIT * 3, 0);
    lv_obj_set_style_bg_opa(light_toggles, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(light_toggles, 0, 0);
    
    toggle_lighting = create_toggle(light_toggles, "Lighting System", true, toggle_cb);
    
    return screen;
}

// Update the climate screen with current values
void ui_climate_update_values(float temp_target, float humidity_target, float light_target,
                           bool heating_on, bool cooling_on, bool humidifier_on, bool lighting_on) {
    // Update sliders with animations
    lv_slider_set_value(slider_temp, (int)temp_target, LV_ANIM_ON);
    lv_slider_set_value(slider_humidity, (int)humidity_target, LV_ANIM_ON);
    lv_slider_set_value(slider_light, (int)light_target, LV_ANIM_ON);
    
    // Update value labels
    lv_label_set_text_fmt(value_temp, "%.1f°C", temp_target);
    lv_label_set_text_fmt(value_humidity, "%.1f%%", humidity_target);
    lv_label_set_text_fmt(value_light, "%.1f%%", light_target);
    
    // Update toggles with visual feedback
    if (heating_on) {
        lv_obj_add_state(toggle_heating, LV_STATE_CHECKED);
        animate_pulse(toggle_heating, 1500);
    } else {
        lv_obj_clear_state(toggle_heating, LV_STATE_CHECKED);
    }
    
    if (cooling_on) {
        lv_obj_add_state(toggle_cooling, LV_STATE_CHECKED);
        animate_pulse(toggle_cooling, 1500);
    } else {
        lv_obj_clear_state(toggle_cooling, LV_STATE_CHECKED);
    }
    
    if (humidifier_on) {
        lv_obj_add_state(toggle_humidifier, LV_STATE_CHECKED);
        animate_pulse(toggle_humidifier, 1500);
    } else {
        lv_obj_clear_state(toggle_humidifier, LV_STATE_CHECKED);
    }
    
    if (lighting_on) {
        lv_obj_add_state(toggle_lighting, LV_STATE_CHECKED);
        animate_pulse(toggle_lighting, 1500);
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
    lv_obj_t *slider = lv_event_get_target(e);
    lv_obj_t *label = lv_event_get_user_data(e);
    int value = lv_slider_get_value(slider);
    lv_label_set_text_fmt(label, "%d°C", value);
}

static void humidity_slider_cb(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);
    lv_obj_t *label = lv_event_get_user_data(e);
    int value = lv_slider_get_value(slider);
    lv_label_set_text_fmt(label, "%d%%", value);
}

static void light_slider_cb(lv_event_t *e) {
    lv_obj_t *slider = lv_event_get_target(e);
    lv_obj_t *label = lv_event_get_user_data(e);
    int value = lv_slider_get_value(slider);
    lv_label_set_text_fmt(label, "%d%%", value);
}

// Toggle switch callback
static void toggle_cb(lv_event_t *e) {
    lv_obj_t *toggle = lv_event_get_target(e);
    bool checked = lv_obj_has_state(toggle, LV_STATE_CHECKED);
    
    if (checked) {
        animate_pulse(toggle, 1500);
    }
}