#include "settings_manager.h"
#include "config.h"
#include <SD.h>
#include <Arduino_JSON.h>

AppSettings settings;

void settings_init() {
    File file = SD.open(SETTINGS_FILE);
    if (!file) {
        save_settings(); // デフォルト設定で作成
        return;
    }
    JSONVar config = JSON.parse(file.readString());
    file.close();
    if (config.hasOwnProperty("startup_screen")) {
        settings.startupScreen = (const char*)config["startup_screen"];
    }
}

void save_settings() {
    JSONVar config;
    config["startup_screen"] = settings.startupScreen;

    File file = SD.open(SETTINGS_FILE, FILE_WRITE);
    if (file) {
        file.print(JSON.stringify(config));
        file.close();
    }
}