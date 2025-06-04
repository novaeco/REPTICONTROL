#include "ui_system.h"
#include "../ui_helpers.h"
#include "esp_log.h"

static const char *TAG = "ui_system";

// Store references for updating
static lv_obj_t *battery_bar;
static lv_obj_t *battery_label;
static lv_obj_t *memory_bar;
static lv_obj_t *memory_label;
static lv_obj_t *cpu_bar;
static lv_obj_t *cpu_label;
static lv_obj_t *heating_led;
static lv_obj_t *cooling_led;
static lv_obj_t *humidifier_led;
static lv_obj_t *lighting_led;

// Callback prototypes
static void reboot_cb(lv_event_t *e);
static void shutdown_cb(lv_event_t *e);
static void test_alert_cb(lv_event_t *e);

// Create the system status screen
lv_obj_t *ui_system_create(void) {
    ESP_LOGI(TAG, "Creating system screen");

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
    lv_label_set_text(title, "System Status");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, GRID_UNIT * 2, 0);

    // Create main content container with grid layout
    lv_obj_t *content = lv_obj_create(screen);
    lv_obj_set_size(content, LV_HOR_RES - SCREEN_PADDING * 2, LV_VER_RES - TOUCH_TARGET_MIN - SCREEN_PADDING * 2);
    lv_obj_align(content, LV_ALIGN_TOP_MID, 0, TOUCH_TARGET_MIN + GRID_UNIT);
    lv_obj_set_grid_dsc_array(content, (lv_coord_t[]){LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST},
                             (lv_coord_t[]){LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST});
    lv_obj_set_style_pad_gap(content, GRID_UNIT * 2, 0);

    // Battery status card
    lv_obj_t *battery_card = create_card(content, "Battery Status", LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(battery_card, 0, 1, 1, LV_GRID_ALIGN_STRETCH, LV_GRID_ALIGN_STRETCH);

    // Battery gauge
    lv_obj_t *battery_gauge = create_gauge(battery_card, "", "%", 0, 100, COLOR_SUCCESS);
    lv_obj_align(battery_gauge, LV_ALIGN_CENTER, 0, GRID_UNIT * 2);

    battery_label = lv_label_create(battery_card);
    lv_label_set_text(battery_label, LV_SYMBOL_BATTERY_FULL " 100%");
    lv_obj_add_style(battery_label, &style_value, 0);
    lv_obj_align(battery_label, LV_ALIGN_BOTTOM_MID, 0, -GRID_UNIT * 2);

    // System resources card
    lv_obj_t *resources_card = create_card(content, "System Resources", LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(resources_card, 0, 2, 1, LV_GRID_ALIGN_STRETCH, LV_GRID_ALIGN_STRETCH);

    // CPU usage
    lv_obj_t *cpu_cont = lv_obj_create(resources_card);
    lv_obj_set_size(cpu_cont, LV_PCT(100), TOUCH_TARGET_MIN);
    lv_obj_set_style_pad_all(cpu_cont, GRID_UNIT, 0);

    lv_obj_t *cpu_title = lv_label_create(cpu_cont);
    lv_label_set_text(cpu_title, "CPU Usage");
    lv_obj_align(cpu_title, LV_ALIGN_LEFT_MID, 0, 0);

    cpu_bar = lv_bar_create(cpu_cont);
    lv_obj_set_size(cpu_bar, LV_PCT(50), GRID_UNIT * 2);
    lv_bar_set_range(cpu_bar, 0, 100);
    lv_obj_align(cpu_bar, LV_ALIGN_RIGHT_MID, -GRID_UNIT * 6, 0);

    cpu_label = lv_label_create(cpu_cont);
    lv_label_set_text(cpu_label, "25%");
    lv_obj_align(cpu_label, LV_ALIGN_RIGHT_MID, -GRID_UNIT, 0);

    // Memory usage
    lv_obj_t *mem_cont = lv_obj_create(resources_card);
    lv_obj_set_size(mem_cont, LV_PCT(100), TOUCH_TARGET_MIN);
    lv_obj_set_style_pad_all(mem_cont, GRID_UNIT, 0);

    lv_obj_t *mem_title = lv_label_create(mem_cont);
    lv_label_set_text(mem_title, "Memory Usage");
    lv_obj_align(mem_title, LV_ALIGN_LEFT_MID, 0, 0);

    memory_bar = lv_bar_create(mem_cont);
    lv_obj_set_size(memory_bar, LV_PCT(50), GRID_UNIT * 2);
    lv_bar_set_range(memory_bar, 0, 100);
    lv_obj_align(memory_bar, LV_ALIGN_RIGHT_MID, -GRID_UNIT * 6, 0);

    memory_label = lv_label_create(mem_cont);
    lv_label_set_text(memory_label, "40%");
    lv_obj_align(memory_label, LV_ALIGN_RIGHT_MID, -GRID_UNIT, 0);

    // Device status card
    lv_obj_t *device_card = create_card(content, "Device Status", LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(device_card, 1, 1, 1, LV_GRID_ALIGN_STRETCH, LV_GRID_ALIGN_STRETCH);

    // Status indicators
    const char *device_names[] = {"Heating", "Cooling", "Humidifier", "Light"};
    const lv_color_t led_colors[] = {COLOR_ERROR, COLOR_INFO, COLOR_INFO, COLOR_WARNING};
    lv_obj_t **leds[] = {&heating_led, &cooling_led, &humidifier_led, &lighting_led};

    for (int i = 0; i < 4; i++) {
        lv_obj_t *status_row = lv_obj_create(device_card);
        lv_obj_set_size(status_row, LV_PCT(100), TOUCH_TARGET_MIN);
        lv_obj_set_style_pad_all(status_row, GRID_UNIT, 0);
        lv_obj_set_flex_flow(status_row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(status_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        lv_obj_t *name_label = lv_label_create(status_row);
        lv_label_set_text(name_label, device_names[i]);

        *leds[i] = create_badge(status_row, "OFF", led_colors[i]);
    }

    // System actions card
    lv_obj_t *actions_card = create_card(content, "System Actions", LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(actions_card, 1, 2, 1, LV_GRID_ALIGN_STRETCH, LV_GRID_ALIGN_STRETCH);

    // Action buttons container
    lv_obj_t *btn_container = lv_obj_create(actions_card);
    lv_obj_set_size(btn_container, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(btn_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(btn_container, GRID_UNIT * 2, 0);
    lv_obj_set_style_pad_top(btn_container, GRID_UNIT * 4, 0);

    // Reboot button
    lv_obj_t *reboot_btn = lv_btn_create(btn_container);
    lv_obj_set_size(reboot_btn, LV_PCT(80), TOUCH_TARGET_MIN);
    lv_obj_add_style(reboot_btn, &style_btn, 0);
    lv_obj_set_style_bg_color(reboot_btn, COLOR_WARNING, 0);
    lv_obj_center(reboot_btn);
    lv_obj_add_event_cb(reboot_btn, reboot_cb, LV_EVENT_CLICKED, NULL);
    add_ripple_effect(reboot_btn);

    lv_obj_t *reboot_label = lv_label_create(reboot_btn);
    lv_label_set_text(reboot_label, LV_SYMBOL_REFRESH " Reboot System");
    lv_obj_center(reboot_label);

    // Shutdown button
    lv_obj_t *shutdown_btn = lv_btn_create(btn_container);
    lv_obj_set_size(shutdown_btn, LV_PCT(80), TOUCH_TARGET_MIN);
    lv_obj_set_style_bg_color(shutdown_btn, COLOR_ERROR, 0);
    lv_obj_center(shutdown_btn);
    lv_obj_add_event_cb(shutdown_btn, shutdown_cb, LV_EVENT_CLICKED, NULL);
    add_ripple_effect(shutdown_btn);

    lv_obj_t *shutdown_label = lv_label_create(shutdown_btn);
    lv_label_set_text(shutdown_label, LV_SYMBOL_POWER " Shutdown");
    lv_obj_center(shutdown_label);

    // Firmware info card
    lv_obj_t *firmware_card = create_card(content, "Firmware Information", LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_grid_cell(firmware_card, 2, 2, 1, LV_GRID_ALIGN_STRETCH, LV_GRID_ALIGN_STRETCH);

    // Info table
    struct {
        const char *label;
        const char *value;
    } info_rows[] = {
        {"Version", "ReptiControl v1.0.0"},
        {"Build Date", "2025-01-15"},
        {"ESP-IDF", "v5.0.1"},
        {"LVGL", "v9.2.2"}
    };

    for (int i = 0; i < 4; i++) {
        lv_obj_t *row = lv_obj_create(firmware_card);
        lv_obj_set_size(row, LV_PCT(100), TOUCH_TARGET_MIN);
        lv_obj_set_style_pad_all(row, GRID_UNIT, 0);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        lv_obj_t *label = lv_label_create(row);
        lv_label_set_text(label, info_rows[i].label);
        lv_obj_add_style(label, &style_text_muted, 0);

        lv_obj_t *value = lv_label_create(row);
        lv_label_set_text(value, info_rows[i].value);
        lv_obj_set_style_text_color(value, COLOR_PRIMARY, 0);
    }

    return screen;
}

// Update system status
void ui_system_update_status(int battery_level, bool heating_on, bool cooling_on,
                           bool humidifier_on, bool lighting_on) {
    // Update status badges with animations
    lv_label_set_text(lv_obj_get_child(heating_led, 0), heating_on ? "ON" : "OFF");
    lv_label_set_text(lv_obj_get_child(cooling_led, 0), cooling_on ? "ON" : "OFF");
    lv_label_set_text(lv_obj_get_child(humidifier_led, 0), humidifier_on ? "ON" : "OFF");
    lv_label_set_text(lv_obj_get_child(lighting_led, 0), lighting_on ? "ON" : "OFF");

    // Add pulse animation to active systems
    if (heating_on) animate_pulse(heating_led, 1500);
    if (cooling_on) animate_pulse(cooling_led, 1500);
    if (humidifier_on) animate_pulse(humidifier_led, 1500);
    if (lighting_on) animate_pulse(lighting_led, 1500);

    // Update battery indicator
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

    lv_label_set_text_fmt(battery_label, "%s %d%%", batt_icon, battery_level);
    lv_obj_set_style_text_color(battery_label, batt_color, 0);
}

// Update memory and CPU usage stats
void ui_system_update_stats(int cpu_usage, int memory_usage) {
    // Update bars with animations
    lv_bar_set_value(cpu_bar, cpu_usage, LV_ANIM_ON);
    lv_bar_set_value(memory_bar, memory_usage, LV_ANIM_ON);

    // Update labels
    lv_label_set_text_fmt(cpu_label, "%d%%", cpu_usage);
    lv_label_set_text_fmt(memory_label, "%d%%", memory_usage);

    // Update colors based on usage levels
    lv_color_t cpu_color = cpu_usage > 80 ? COLOR_ERROR :
                          cpu_usage > 60 ? COLOR_WARNING : COLOR_SUCCESS;

    lv_color_t mem_color = memory_usage > 80 ? COLOR_ERROR :
                          memory_usage > 60 ? COLOR_WARNING : COLOR_SUCCESS;

    lv_obj_set_style_bg_color(cpu_bar, cpu_color, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(memory_bar, mem_color, LV_PART_INDICATOR);
}

// Simulate system reboot
void ui_system_reboot(void) {
    static const char *btns[] = {"Yes", "No", ""};

    lv_obj_t *mbox = lv_msgbox_create(NULL, "Confirm Reboot",
                                     "Are you sure you want to reboot the system?", btns, false);
    lv_obj_add_style(mbox, &style_alert_box, 0);
    lv_obj_center(mbox);

    lv_obj_add_event_cb(mbox, [](lv_event_t *e) {
        const char *btn_text = lv_msgbox_get_active_btn_text(lv_event_get_current_target(e));
        if (strcmp(btn_text, "Yes") == 0) {
            // Show reboot animation
            lv_obj_t *reboot_screen = lv_obj_create(NULL);
            lv_obj_set_style_bg_color(reboot_screen, COLOR_PRIMARY_DARK, 0);
            lv_scr_load(reboot_screen);

            lv_obj_t *spinner = lv_spinner_create(reboot_screen, 1000, 60);
            lv_obj_set_size(spinner, 100, 100);
            lv_obj_center(spinner);

            lv_obj_t *label = lv_label_create(reboot_screen);
            lv_label_set_text(label, "Rebooting...");
            lv_obj_set_style_text_color(label, COLOR_SURFACE, 0);
            lv_obj_align(label, LV_ALIGN_CENTER, 0, 60);
        }
        lv_msgbox_close(lv_event_get_current_target(e));
    }, LV_EVENT_VALUE_CHANGED, NULL);
}

// Simulate system shutdown
void ui_system_shutdown(void) {
    static const char *btns[] = {"Yes", "No", ""};

    lv_obj_t *mbox = lv_msgbox_create(NULL, "Confirm Shutdown",
                                     "Are you sure you want to shut down the system?", btns, false);
    lv_obj_add_style(mbox, &style_alert_box, 0);
    lv_obj_center(mbox);

    lv_obj_add_event_cb(mbox, [](lv_event_t *e) {
        const char *btn_text = lv_msgbox_get_active_btn_text(lv_event_get_current_target(e));
        if (strcmp(btn_text, "Yes") == 0) {
            // Show shutdown animation
            lv_obj_t *shutdown_screen = lv_obj_create(NULL);
            lv_obj_set_style_bg_color(shutdown_screen, COLOR_PRIMARY_DARK, 0);
            lv_scr_load(shutdown_screen);

            lv_obj_t *label = lv_label_create(shutdown_screen);
            lv_label_set_text(label, "Shutting down...");
            lv_obj_set_style_text_color(label, COLOR_SURFACE, 0);
            lv_obj_center(label);

            // Fade out animation
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, shutdown_screen);
            lv_anim_set_time(&a, 2000);
            lv_anim_set_values(&a, LV_OPA_COVER, LV_OPA_TRANSP);
            lv_anim_set_exec_cb(&a, [](void *obj, int32_t v) {
                lv_obj_set_style_bg_opa((lv_obj_t *)obj, v, 0);
            });
            lv_anim_start(&a);
        }
        lv_msgbox_close(lv_event_get_current_target(e));
    }, LV_EVENT_VALUE_CHANGED, NULL);
}

// Button callbacks
static void reboot_cb(lv_event_t *e) {
    ui_system_reboot();
}

static void shutdown_cb(lv_event_t *e) {
    ui_system_shutdown();
}

static void test_alert_cb(lv_event_t *e) {
    static const char *btns[] = {"OK", ""};

    lv_obj_t *mbox = lv_msgbox_create(NULL, "Test Alert",
                                     "This is a test alert message.\nIn a real system, alerts would be\ntriggered by system events.",
                                     btns, false);
    lv_obj_add_style(mbox, &style_alert_box, 0);
    lv_obj_center(mbox);
}
