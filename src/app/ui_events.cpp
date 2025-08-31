#include "ui_events.h"
#include "app/settings_manager.h"
#include "hal/sd_card_handler.h"
#include "ui/ui.h"

std::vector<lv_obj_t*> log_list_buttons;

void DropdownStartupScreenChange(lv_event_t * e) {
    char buf[32];
    lv_dropdown_get_selected_str(ui_startupscreen, buf, sizeof(buf));
    if (strcmp(buf, "Home") == 0) settings.startupScreen = "home";
    else if (strcmp(buf, "Raw Mode") == 0) settings.startupScreen = "raw_mode";
    else if (strcmp(buf, "Analytics Mode") == 0) settings.startupScreen = "analytics_mode";
    save_settings();
}


static void log_list_event_handler(lv_event_t * e) {
    lv_obj_t* target = lv_event_get_target(e);
    const char* file_name_cstr = lv_list_get_btn_text(ui_loglist, target);
    String file_name = "/" + String(file_name_cstr);
    String content = read_log_file_content(file_name);
    lv_textarea_set_text(ui_SerialPloter, content.c_str());
}

// --- 初期化 ---
// void ui_events_init() {
//     // SquareLine Studioのイベント機能で Call Function を使った場合は不要
//     // 手動で関連付ける場合はここに記述する
//     lv_obj_add_event_cb(ui_settings_dropdown, settings_dropdown_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
//     lv_obj_add_event_cb(ui_loglist, log_list_event_handler, LV_EVENT_CLICKED, NULL);
// }