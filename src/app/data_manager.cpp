#include "data_manager.h"
#include "hal/sd_card_handler.h"
#include "ui/ui.h" // UIオブジェクトを直接操作するため

static bool isIndexReceived = false;

// 簡易的なインデックス行判定
bool is_index_line(const String& data) {
    if (data.indexOf(',') > 0) {
        String firstElement = data.substring(0, data.indexOf(','));
        firstElement.trim();
        return (firstElement.length() > 0 && !isDigit(firstElement.charAt(0)));
    }
    return false;
}

void process_incoming_data(const String& data) {
    // 1. SDカードに全データをログとして保存
    log_to_sd(data);

    // 2. UIの更新
    // Rawモード画面への追記
    lv_textarea_add_text(ui_SerialPloter, data.c_str());
    lv_textarea_add_text(ui_SerialPloter, "\n");

    // Analyticsモード画面の更新
    if (is_index_line(data)) {
        if (isIndexReceived) {
            create_new_log_file(); // 2回目以降のインデックスなら新ファイル
        }
        isIndexReceived = true;
        // TODO: 解析モードのテーブルを再構築する関数を呼び出す
        // update_analytics_table(data.c_str());
    } else if (isIndexReceived) {
        // TODO: 解析モードのテーブルの値を更新する関数を呼び出す
        // update_analytics_values(data.c_str());
    }
}