#include <Arduino.h>
#include <SD.h>

#define SD_CS_PIN 5  ///< SDカードのCS (Chip Select) ピン

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // SD card を使う場合の初期化コードをここに追加
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized successfully.");

  // ファイルの書き込みコードをここに追加
  File file = SD.open("/test.txt", FILE_WRITE);
  if (file) {
    file.println("Hello, SD card!");
    file.close();
    Serial.println("File written successfully.");
  } else {
    Serial.println("Failed to open file for writing.");
  }

  // 先程書き込んだファイルの読み込みコードをここに追加
  File readFile = SD.open("/test.txt");
  if (readFile) {
    Serial.println("Reading from file:");
    while (readFile.available()) {
      Serial.write(readFile.read());
    }
    readFile.close();
  } else {
    Serial.println("Failed to open file for reading.");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}