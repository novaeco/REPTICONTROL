#include "ui_first_setup.h"
#include "ui_helpers.h"
#include "drivers/display_config.h"
#include "core/settings_manager.h"
#include "core/network_manager.h"
#include "nvs.h"
#include "nvs_flash.h"

static lv_obj_t *screen;
static lv_obj_t *ssid_ta;
static lv_obj_t *pass_ta;
static lv_obj_t *disp_dd;
static bool setup_complete = false;

static void save_event_handler(lv_event_t *e) {
    display_type_t type = lv_dropdown_get_selected(disp_dd) == 0 ? DISPLAY_5_INCH : DISPLAY_7_INCH;
    display_config_set_type(type);
    settings_set_int("display_type", (int)type);
    settings_set_int("wifi_mode", RC_WIFI_MODE_STA);
    const char *ssid = lv_textarea_get_text(ssid_ta);
    const char *pass = lv_textarea_get_text(pass_ta);
    settings_set_int("wifi_config_set", 1);
    // Save strings
    nvs_handle_t h; // use low-level NVS
    if (nvs_open("settings", NVS_READWRITE, &h) == ESP_OK) {
        nvs_set_str(h, "wifi_ssid", ssid);
        nvs_set_str(h, "wifi_password", pass);
        nvs_commit(h);
        nvs_close(h);
    }
    settings_save();
    setup_complete = true;
    lv_obj_del(screen);
}

void ui_first_setup_create(void) {
    screen = lv_obj_create(NULL);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t *label = lv_label_create(screen);
    lv_label_set_text(label, "Configuration initiale");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10);

    disp_dd = lv_dropdown_create(screen);
    lv_dropdown_set_options(disp_dd, "5 pouces\n7 pouces");
    lv_obj_align(disp_dd, LV_ALIGN_TOP_MID, 0, 40);

    ssid_ta = lv_textarea_create(screen);
    lv_textarea_set_placeholder_text(ssid_ta, "SSID WiFi");
    lv_obj_set_width(ssid_ta, 200);
    lv_obj_align(ssid_ta, LV_ALIGN_TOP_MID, 0, 80);

    pass_ta = lv_textarea_create(screen);
    lv_textarea_set_placeholder_text(pass_ta, "Mot de passe");
    lv_obj_set_width(pass_ta, 200);
    lv_textarea_set_password_mode(pass_ta, true);
    lv_obj_align(pass_ta, LV_ALIGN_TOP_MID, 0, 120);

    lv_obj_t *kb = lv_keyboard_create(screen);
    lv_keyboard_set_textarea(kb, ssid_ta);
    lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_obj_t *btn = lv_btn_create(screen);
    lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 160);
    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Enregistrer");
    lv_obj_center(btn_label);
    lv_obj_add_event_cb(btn, save_event_handler, LV_EVENT_CLICKED, NULL);

    lv_scr_load(screen);
}

bool ui_first_setup_is_complete(void) {
    return setup_complete;
}
