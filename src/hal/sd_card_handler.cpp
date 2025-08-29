#include "sd_card_handler.h"
#include "../config.h"
#include "ui/ui.h" // UIオブジェクトを直接操作するため
#include <SD.h>
#include <vector>

static String currentLogFile;
extern std::vector<lv_obj_t*> log_list_buttons; // ui_events.cppで定義

void sd_card_init() {
    if (!SD.begin(SD_CS_PIN)) {
        // エラー処理: UIに表示するなど
        lv_label_set_text(ui_status_label, "SD Card Mount Failed!");
        while (1);
    }
    create_new_log_file();
}

void create_new_log_file() {
    int fileNumber = 0;
    while (true) {
        fileNumber++;
        char fileName[30];
        sprintf(fileName, "%s%03d%s", LOG_FILE_PREFIX, fileNumber, LOG_FILE_EXTENSION);
        if (!SD.exists(fileName)) {
            currentLogFile = String(fileName);
            break;
        }
    }
    // UIのステータスラベルに現在のログファイル名を表示
    lv_label_set_text_fmt(ui_status_label, "Logging to: %s", currentLogFile.c_str());
}

void log_to_sd(const String& data) {
    File file = SD.open(currentLogFile, FILE_APPEND);
    if (file) {
        file.println(String(millis()) + "," + data);
        file.close();
    }
}

void populate_log_list() {
    // 既存リストのクリア
    for (auto btn : log_list_buttons) {
        lv_obj_del(btn);
    }
    log_list_buttons.clear();

    File root = SD.open("/");
    File file = root.openNextFile();
    while (file) {
        String fileName = file.name();
        if (fileName.startsWith(LOG_FILE_PREFIX) && fileName.endsWith(LOG_FILE_EXTENSION)) {
            lv_obj_t* btn = lv_list_add_btn(ui_log_list, LV_SYMBOL_FILE, fileName.c_str());
            log_list_buttons.push_back(btn);
        }
        file.close();
        file = root.openNextFile();
    }
    root.close();
}

String read_log_file_content(const String& filename) {
    File file = SD.open(filename, FILE_READ);
    if (!file) {
        return "Error: Failed to open file.";
    }
    String content = "";
    int line_count = 0;
    while (file.available() && line_count < 50) { // 最大50行まで
        content += file.readStringUntil('\n') + "\n";
        line_count++;
    }
    file.close();
    return content;
}