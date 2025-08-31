#include "sd_card_handler.h"
#include "config.h"
#include "ui/ui.h" // UIオブジェクトを直接操作するため
#include <SD.h>
#include <vector>


// static String currentLogFile;
String currentLogFile;
extern std::vector<lv_obj_t*> log_list_buttons; // ui_events.cppで定義

SPIClass SPI_SD(VSPI);

void sd_card_init() {
    enum { spi_sck = 18, spi_miso = 19, spi_mosi = 23, spi_ss = 5 };
    SPI_SD.begin(spi_sck, spi_miso, spi_mosi, SD_CS_PIN);
    if (!SD.begin(SD_CS_PIN, SPI_SD)) {
        Serial.println("Card Mount Failed");
        // lv_label_set_text(ui_statuslabel, "SD Card Mount Failed!");
        while (1);
    }
    // Serial.println("SD Card Mount Successful");
    // create_new_log_file();
}

void create_new_log_file() {
    int fileNumber = 0;
    while (true) {
        fileNumber++;
        char fileName[30];
        sprintf(fileName, "%s%03d%s", LOG_FILE_PREFIX, fileNumber, LOG_FILE_EXTENSION);
        Serial.println(fileName);
        if (!SD.exists(fileName)) {
            currentLogFile = String(fileName);
            Serial.println("New log file created: " + currentLogFile);
            break;
        }
    }
    // UIのステータスラベルに現在のログファイル名を表示
    // lv_label_set_text_fmt(ui_statuslabel, "Logging to: %s", currentLogFile.c_str());
}

void log_to_sd(const String& data) {
    sd_card_init();
    File file = SD.open(currentLogFile, FILE_APPEND);
    if (file) {
        file.println(String(millis()) + "," + data);
        file.close();
    }
    // lcd.initBus();
}

/**
 * @brief ログファイル一覧をUIのリストに反映する
 */
void populate_log_list() {
    // 既存リストのクリア
    // for (auto btn : log_list_buttons) {
    //     lv_obj_del(btn);
    // }
    // log_list_buttons.clear();

    sd_card_init();
    File root = SD.open("/");
    File file = root.openNextFile();
    String logFiles = "Realtime\n"; // 最初に Realtime を追加
    while (file) {
        String filePath = file.path();
        if (filePath.startsWith(LOG_FILE_PREFIX) && filePath.endsWith(LOG_FILE_EXTENSION)) {
            // lv_obj_t* btn = lv_list_add_btn(ui_loglist, LV_SYMBOL_FILE, filePath.c_str());
            // log_list_buttons.push_back(btn);
            String fileName = file.name();
            logFiles += fileName + "\n";
        }
        file.close();
        file = root.openNextFile();
    }
    if (!logFiles.isEmpty()) { // logFilesの最後の改行を削除
        logFiles.remove(logFiles.length() - 1);
    }
    lv_roller_set_options(ui_loglist, logFiles.c_str(), LV_ROLLER_MODE_NORMAL);
    root.close();
    // lcd.initBus();
}

String read_log_file_content(const String& filename) {
    sd_card_init();
    if (!SD.exists(filename)) {
        // return "Error: Log file does not exist.";
        return "";
    }
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
    // lcd.initBus();
    return content;
}