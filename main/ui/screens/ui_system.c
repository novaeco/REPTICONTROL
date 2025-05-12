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
    lv_label_set_text(title, "System Status");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);
    lv_obj_align(title, LV_ALIGN_LEFT_MID, 16, 0);
    
    // Create flex container for two columns
    lv_obj_t *content = lv_obj_create(screen);
    lv_obj_set_size(content, LV_HOR_RES - 32, LV_VER_RES - 128);
    lv_obj_align(content, LV_ALIGN_TOP_MID, 0, 60);
    lv_obj_set_flex_flow(content, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_bg_opa(content, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(content, 0, 0);
    lv_obj_set_style_pad_gap(content, 16, 0);
    
    // Left column - System status
    lv_obj_t *left_col = lv_obj_create(content);
    lv_obj_set_size(left_col, LV_PCT(50), LV_PCT(100));
    lv_obj_set_style_bg_opa(left_col, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(left_col, 0, 0);
    lv_obj_set_flex_flow(left_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(left_col, 16, 0);
    
    // Battery status card
    lv_obj_t *battery_card = create_card(left_col, "Battery Status", LV_PCT(100), 100);
    
    // Battery bar
    lv_obj_t *battery_cont = lv_obj_create(battery_card);
    lv_obj_set_size(battery_cont, LV_PCT(100), 50);
    lv_obj_set_style_bg_opa(battery_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(battery_cont, 0, 0);
    lv_obj_set_style_pad_top(battery_cont, 30, 0);
    
    battery_bar = lv_bar_create(battery_cont);
    lv_obj_set_size(battery_bar, LV_PCT(90), 20);
    lv_bar_set_range(battery_bar, 0, 100);
    lv_bar_set_value(battery_bar, 80, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(battery_bar, lv_color_hex(0xE0E0E0), LV_PART_MAIN);
    lv_obj_set_style_bg_color(battery_bar, COLOR_SUCCESS, LV_PART_INDICATOR);
    lv_obj_align(battery_bar, LV_ALIGN_CENTER, 0, 0);
    
    battery_label = lv_label_create(battery_cont);
    lv_label_set_text(battery_label, "80%");
    lv_obj_align(battery_label, LV_ALIGN_BOTTOM_RIGHT, -5, 10);
    
    // System resources card
    lv_obj_t *resources_card = create_card(left_col, "System Resources", LV_PCT(100), 160);
    
    // Memory usage
    lv_obj_t *memory_cont = lv_obj_create(resources_card);
    lv_obj_set_size(memory_cont, LV_PCT(100), 40);
    lv_obj_set_style_bg_opa(memory_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(memory_cont, 0, 0);
    lv_obj_set_style_pad_top(memory_cont, 30, 0);
    
    lv_obj_t *mem_title = lv_label_create(memory_cont);
    lv_label_set_text(mem_title, "Memory:");
    lv_obj_align(mem_title, LV_ALIGN_LEFT_MID, 5, 0);
    
    memory_bar = lv_bar_create(memory_cont);
    lv_obj_set_size(memory_bar, LV_PCT(65), 15);
    lv_bar_set_range(memory_bar, 0, 100);
    lv_bar_set_value(memory_bar, 40, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(memory_bar, lv_color_hex(0xE0E0E0), LV_PART_MAIN);
    lv_obj_set_style_bg_color(memory_bar, COLOR_PRIMARY, LV_PART_INDICATOR);
    lv_obj_align(memory_bar, LV_ALIGN_RIGHT_MID, -40, 0);
    
    memory_label = lv_label_create(memory_cont);
    lv_label_set_text(memory_label, "40%");
    lv_obj_align(memory_label, LV_ALIGN_RIGHT_MID, -5, 0);
    
    // CPU usage
    lv_obj_t *cpu_cont = lv_obj_create(resources_card);
    lv_obj_set_size(cpu_cont, LV_PCT(100), 40);
    lv_obj_set_style_bg_opa(cpu_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(cpu_cont, 0, 0);
    
    lv_obj_t *cpu_title = lv_label_create(cpu_cont);
    lv_label_set_text(cpu_title, "CPU:");
    lv_obj_align(cpu_title, LV_ALIGN_LEFT_MID, 5, 0);
    
    cpu_bar = lv_bar_create(cpu_cont);
    lv_obj_set_size(cpu_bar, LV_PCT(65), 15);
    lv_bar_set_range(cpu_bar, 0, 100);
    lv_bar_set_value(cpu_bar, 25, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(cpu_bar, lv_color_hex(0xE0E0E0), LV_PART_MAIN);
    lv_obj_set_style_bg_color(cpu_bar, COLOR_PRIMARY, LV_PART_INDICATOR);
    lv_obj_align(cpu_bar, LV_ALIGN_RIGHT_MID, -40, 0);
    
    cpu_label = lv_label_create(cpu_cont);
    lv_label_set_text(cpu_label, "25%");
    lv_obj_align(cpu_label, LV_ALIGN_RIGHT_MID, -5, 0);
    
    // Action buttons
    lv_obj_t *action_card = create_card(left_col, "System Actions", LV_PCT(100), 100);
    
    lv_obj_t *btn_row = lv_obj_create(action_card);
    lv_obj_set_size(btn_row, LV_PCT(100), 50);
    lv_obj_set_flex_flow(btn_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_row, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(btn_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btn_row, 0, 0);
    lv_obj_set_style_pad_top(btn_row, 30, 0);
    
    // Reboot button
    lv_obj_t *reboot_btn = lv_btn_create(btn_row);
    lv_obj_set_size(reboot_btn, 100, 40);
    lv_obj_add_style(reboot_btn, &style_btn, 0);
    lv_obj_set_style_bg_color(reboot_btn, COLOR_WARNING, 0);
    lv_obj_add_event_cb(reboot_btn, reboot_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *reboot_label = lv_label_create(reboot_btn);
    lv_label_set_text(reboot_label, "Reboot");
    lv_obj_center(reboot_label);
    
    // Shutdown button
    lv_obj_t *shutdown_btn = lv_btn_create(btn_row);
    lv_obj_set_size(shutdown_btn, 100, 40);
    lv_obj_set_style_bg_color(shutdown_btn, COLOR_ERROR, 0);
    lv_obj_add_event_cb(shutdown_btn, shutdown_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *shutdown_label = lv_label_create(shutdown_btn);
    lv_label_set_text(shutdown_label, "Shutdown");
    lv_obj_center(shutdown_label);
    
    // Test alert button (for demonstration)
    lv_obj_t *test_btn = lv_btn_create(action_card);
    lv_obj_set_size(test_btn, 100, 30);
    lv_obj_set_style_bg_color(test_btn, COLOR_NEUTRAL, 0);
    lv_obj_align(test_btn, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_obj_add_event_cb(test_btn, test_alert_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *test_label = lv_label_create(test_btn);
    lv_label_set_text(test_label, "Test Alert");
    lv_obj_center(test_label);
    
    // Right column - Device status
    lv_obj_t *right_col = lv_obj_create(content);
    lv_obj_set_size(right_col, LV_PCT(50), LV_PCT(100));
    lv_obj_set_style_bg_opa(right_col, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(right_col, 0, 0);
    lv_obj_set_flex_flow(right_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(right_col, 16, 0);
    
    // Device status card
    lv_obj_t *device_card = create_card(right_col, "Device Status", LV_PCT(100), 200);
    
    // Status table layout
    lv_obj_t *status_grid = lv_obj_create(device_card);
    lv_obj_set_size(status_grid, LV_PCT(100), 140);
    lv_obj_set_style_pad_all(status_grid, 0, 0);
    lv_obj_set_style_bg_opa(status_grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(status_grid, 0, 0);
    lv_obj_set_style_pad_top(status_grid, 40, 0);
    lv_obj_set_flex_flow(status_grid, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(status_grid, 10, 0);
    
    // Create status indicators for each system
    static const char *device_names[] = {"Heating System", "Cooling System", "Humidifier", "Lighting"};
    static lv_obj_t **leds[] = {&heating_led, &cooling_led, &humidifier_led, &lighting_led};
    static const lv_color_t led_colors[] = {
        lv_color_hex(0xE53935), // Red for heating
        lv_color_hex(0x2196F3), // Blue for cooling
        lv_color_hex(0x00ACC1), // Cyan for humidifier
        lv_color_hex(0xFFB300)  // Amber for lighting
    };
    
    for (int i = 0; i < 4; i++) {
        lv_obj_t *row = lv_obj_create(status_grid);
        lv_obj_set_size(row, LV_PCT(100), 25);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row, 0, 0);
        lv_obj_set_style_pad_all(row, 0, 0);
        
        // Device name
        lv_obj_t *name_label = lv_label_create(row);
        lv_label_set_text(name_label, device_names[i]);
        lv_obj_align(name_label, LV_ALIGN_LEFT_MID, 10, 0);
        
        // Status LED
        *leds[i] = lv_led_create(row);
        lv_obj_set_size(*leds[i], 20, 20);
        lv_led_set_color(*leds[i], led_colors[i]);
        lv_led_off(*leds[i]); // Start inactive
        lv_obj_align(*leds[i], LV_ALIGN_RIGHT_MID, -10, 0);
    }
    
    // Firmware info card
    lv_obj_t *firmware_card = create_card(right_col, "Firmware Information", LV_PCT(100), 150);
    
    // Info table
    lv_obj_t *info_table = lv_obj_create(firmware_card);
    lv_obj_set_size(info_table, LV_PCT(100), 100);
    lv_obj_set_style_bg_opa(info_table, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(info_table, 0, 0);
    lv_obj_set_style_pad_all(info_table, 0, 0);
    lv_obj_set_style_pad_top(info_table, 40, 0);
    lv_obj_set_flex_flow(info_table, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(info_table, 8, 0);
    
    // Create info rows
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
        lv_obj_t *row = lv_obj_create(info_table);
        lv_obj_set_size(row, LV_PCT(100), 20);
        lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
        lv_obj_set_style_border_width(row, 0, 0);
        lv_obj_set_style_pad_all(row, 0, 0);
        
        lv_obj_t *label = lv_label_create(row);
        lv_label_set_text(label, info_rows[i].label);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0);
        lv_obj_align(label, LV_ALIGN_LEFT_MID, 10, 0);
        
        lv_obj_t *value = lv_label_create(row);
        lv_label_set_text(value, info_rows[i].value);
        lv_obj_set_style_text_font(value, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(value, COLOR_PRIMARY, 0);
        lv_obj_align(value, LV_ALIGN_RIGHT_MID, -10, 0);
    }
    
    return screen;
}

// Update system status
void ui_system_update_status(int battery_level, bool heating_on, bool cooling_on, 
                           bool humidifier_on, bool lighting_on) {
    // Update battery indicator
    lv_bar_set_value(battery_bar, battery_level, LV_ANIM_ON);
    lv_label_set_text_fmt(battery_label, "%d%%", battery_level);
    
    // Update color based on level
    lv_color_t battery_color;
    if (battery_level > 60) {
        battery_color = COLOR_SUCCESS;
    } else if (battery_level > 20) {
        battery_color = COLOR_WARNING;
    } else {
        battery_color = COLOR_ERROR;
    }
    lv_obj_set_style_bg_color(battery_bar, battery_color, LV_PART_INDICATOR);
    
    // Update status LEDs
    if (heating_on) {
        lv_led_on(heating_led);
    } else {
        lv_led_off(heating_led);
    }
    
    if (cooling_on) {
        lv_led_on(cooling_led);
    } else {
        lv_led_off(cooling_led);
    }
    
    if (humidifier_on) {
        lv_led_on(humidifier_led);
    } else {
        lv_led_off(humidifier_led);
    }
    
    if (lighting_on) {
        lv_led_on(lighting_led);
    } else {
        lv_led_off(lighting_led);
    }
}

// Update memory and CPU usage stats
void ui_system_update_stats(int cpu_usage, int memory_usage) {
    // Update bars
    lv_bar_set_value(cpu_bar, cpu_usage, LV_ANIM_ON);
    lv_bar_set_value(memory_bar, memory_usage, LV_ANIM_ON);
    
    // Update labels
    lv_label_set_text_fmt(cpu_label, "%d%%", cpu_usage);
    lv_label_set_text_fmt(memory_label, "%d%%", memory_usage);
    
    // Update colors based on usage levels
    lv_color_t cpu_color, mem_color;
    
    if (cpu_usage > 80) {
        cpu_color = COLOR_ERROR;
    } else if (cpu_usage > 60) {
        cpu_color = COLOR_WARNING;
    } else {
        cpu_color = COLOR_PRIMARY;
    }
    
    if (memory_usage > 80) {
        mem_color = COLOR_ERROR;
    } else if (memory_usage > 60) {
        mem_color = COLOR_WARNING;
    } else {
        mem_color = COLOR_PRIMARY;
    }
    
    lv_obj_set_style_bg_color(cpu_bar, cpu_color, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(memory_bar, mem_color, LV_PART_INDICATOR);
}

// Simulate system reboot
void ui_system_reboot(void) {
    // This would trigger an actual reboot in a real system
    // For simulation, we'll just show a message
    static const char *btns[] = {"OK", ""};
    
    lv_obj_t *mbox = lv_msgbox_create(NULL, "System Reboot", "Simulating system reboot...", btns, false);
    lv_obj_center(mbox);
    
    // Auto-close after delay
    lv_timer_t *timer = lv_timer_create([](lv_timer_t *t) {
        lv_msgbox_close((lv_obj_t *)t->user_data);
    }, 2000, mbox);
    lv_timer_set_repeat_count(timer, 1);
}

// Simulate system shutdown
void ui_system_shutdown(void) {
    // This would trigger an actual shutdown in a real system
    // For simulation, we'll just show a message
    static const char *btns[] = {"OK", ""};
    
    lv_obj_t *mbox = lv_msgbox_create(NULL, "System Shutdown", "Simulating system shutdown...", btns, false);
    lv_obj_center(mbox);
    
    // Auto-close after delay
    lv_timer_t *timer = lv_timer_create([](lv_timer_t *t) {
        lv_msgbox_close((lv_obj_t *)t->user_data);
    }, 2000, mbox);
    lv_timer_set_repeat_count(timer, 1);
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
    
    lv_obj_t *mbox = lv_msgbox_create(NULL, "Test Alert", "This is a test alert message.\nIn a real system, alerts would be\ntriggered by system events.", btns, false);
    lv_obj_center(mbox);
}