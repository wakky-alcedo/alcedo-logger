#include "settings_manager.h"
#include "config.h"
#include <SD.h>
// #include <Arduino_JSON.h>
#include <ArduinoJson.h>
#include <hal/sd_card_handler.h>

AppSettings settings;

void settings_init() {
    sd_card_init();
    File file = SD.open(SETTINGS_FILE);
    if (!file) {
        save_settings(); // デフォルト設定で作成
        Serial.println("Settings file created");
        return;
    }
    DynamicJsonDocument config(1024);
    deserializeJson(config, file.readString());
    file.close();
    if (config.containsKey("startup_screen")) {
        settings.startupScreen = (const char*)config["startup_screen"];
    }
    lcd.initBus();
}

void save_settings() {
    DynamicJsonDocument config(1024);
    config["startup_screen"] = settings.startupScreen;

    sd_card_init();
    File file = SD.open(SETTINGS_FILE, FILE_WRITE);
    if (file) {
        serializeJson(config, file);
        file.close();
    }
    lcd.initBus();
}