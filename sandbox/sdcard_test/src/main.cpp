#include <Arduino.h>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

#include "SD.h"

SPIClass SPI_SD(VSPI);

void sd_dir(void) {
  enum { spi_sck = 18, spi_miso = 19, spi_mosi = 23, spi_ss = 5 };

  SPI_SD.begin(spi_sck, spi_miso, spi_mosi, spi_ss);
  SD.begin(spi_ss, SPI_SD);
  if (!SD.begin(spi_ss, SPI_SD)) {
    Serial.println("Card Mount Failed");
  }

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("None SD Card");
    return;
  }
  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  File root = SD.open("/");
  if (root) {
    File file = root.openNextFile();
    while (file) {
      time_t t = file.getLastWrite();
      struct tm *tm_t = localtime(&t);
      Serial.printf("%04d-%02d-%02d %02d:%02d:%02d ", tm_t->tm_year + 1900, tm_t->tm_mon + 1, tm_t->tm_mday, tm_t->tm_hour, tm_t->tm_min, tm_t->tm_sec);
      if (file.isDirectory()) {
        Serial.print("<DIR>");
        Serial.print("           ");
        Serial.println(file.name());
      } else {
        // ファイル名とファイルサイズを出力
        Serial.print("     ");
        Serial.printf("%10d ", file.size());
        Serial.println(file.name());
      }
      file = root.openNextFile();
    }
  }
}

class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ILI9341 _panel_instance;
    lgfx::Bus_SPI       _bus_instance;
    lgfx::Light_PWM     _light_instance;
    lgfx::Touch_XPT2046 _touch_instance;
  public:
    LGFX(void)
    {
      {
        auto cfg = _bus_instance.config();

        // SPIバスの設定
        cfg.spi_host         = VSPI_HOST;
        cfg.spi_mode         = 0;
        cfg.freq_write       = 40000000;
        cfg.freq_read        = 16000000;
        cfg.spi_3wire        = true;
        cfg.use_lock         = true;
        cfg.dma_channel      = SPI_DMA_CH_AUTO;
        cfg.pin_sclk         = 14; // 変更
        cfg.pin_mosi         = 13; // 変更
        cfg.pin_miso         = 12; // 変更
        cfg.pin_dc           =  2; // 変更

        _bus_instance.config(cfg);
        _panel_instance.setBus(&_bus_instance);
      }

      {
        auto cfg = _panel_instance.config();

        cfg.pin_cs           =    15; // 変更
        cfg.pin_rst          =    -1; // 変更
        cfg.pin_busy         =    -1; // 変更

        cfg.panel_width      =   240;
        cfg.panel_height     =   320;
        cfg.offset_x         =     0;
        cfg.offset_y         =     0;
        cfg.offset_rotation  =     0;
        cfg.dummy_read_pixel =     8;
        cfg.dummy_read_bits  =     1;
        cfg.readable         =  true;
        cfg.invert           = false;
        cfg.rgb_order        = false;
        cfg.dlen_16bit       = false;
        cfg.bus_shared       = false; // 変更

        _panel_instance.config(cfg);
      }

      {
        auto cfg = _light_instance.config();

        cfg.pin_bl = 21;              // 変更
        cfg.invert = false;
        cfg.freq   = 44100;
        cfg.pwm_channel = 7;

        _light_instance.config(cfg);
        _panel_instance.setLight(&_light_instance);
      }

      {
        auto cfg = _touch_instance.config();

        cfg.x_min      =  300;  // 変更
        cfg.x_max      = 3900;  // 変更
        cfg.y_min      = 3700;  // 変更
        cfg.y_max      =  200;  // 変更
        cfg.pin_int    = -1;    // 変更
        cfg.bus_shared = false; // 変更
        cfg.offset_rotation = 0;

        cfg.spi_host = HSPI_HOST; // 変更
        cfg.freq = 1000000;
        cfg.pin_sclk = 25;        // 変更
        cfg.pin_mosi = 32;        // 変更
        cfg.pin_miso = 39;        // 変更
        cfg.pin_cs   = 33;        // 変更

        _touch_instance.config(cfg);
        _panel_instance.setTouch(&_touch_instance);
      }

      setPanel(&_panel_instance);
    }
};

LGFX display;

void setup(void) {
  Serial.begin(115200);
  delay(1000);

  display.init();

  display.setTextSize((std::max(display.width(), display.height()) + 255) >> 8);

  display.fillScreen(TFT_BLACK);
}

uint32_t count = ~0;
void loop(void) {
  display.startWrite();
  display.setRotation(++count & 7);
  display.setColorDepth((count & 8) ? 16 : 24);

  display.setTextColor(TFT_WHITE);
  display.drawNumber(display.getRotation(), 16, 0);

  display.setTextColor(0xFF0000U);
  display.drawString("R", 30, 16);
  display.setTextColor(0x00FF00U);
  display.drawString("G", 40, 16);
  display.setTextColor(0x0000FFU);
  display.drawString("B", 50, 16);

  display.drawRect(30, 30, display.width() - 60, display.height() - 60, count * 7);
  display.drawFastHLine(0, 0, 10);

  display.endWrite();

  auto start = millis();

  while ( (millis() - start) < 1000) {
    int32_t x, y;
    if (display.getTouch(&x, &y)) {
      display.fillRect(x - 2, y - 2, 5, 5, TFT_RED);
    }
  }

  sd_dir();

  display.initBus();
}