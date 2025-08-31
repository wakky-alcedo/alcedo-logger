#include <lvgl.h>
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <hal/LGFX_Config.hpp>
#include <hal/display_manager.h>
#include <hal/sd_card_handler.h>
#include <hal/uart_handler.h>
#include <app/settings_manager.h>
#include <app/data_manager.h>
#include <app/ui_events.h>
#include <ui/ui.h>

/*Don't forget to set Sketchbook location in File/Preferences to the path of your UI project (the parent foder of this INO file)*/

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 320;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

LGFX tft;

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

void setup() {
    Serial.begin( 115200 ); /* prepare for possible serial debug */
    Serial.println("Setup started");

    // --- 初期化処理 ---
    display_init();       // ディスプレイとLVGL
    Serial.println("Display initialized");
    sd_card_init();       // SDカード (display_initの後)
    Serial.println("SD Card initialized");
    settings_init();      // 設定 (sd_card_initの後)
    Serial.println("Settings initialized");

    ui_init();            // UIオブジェクトを作成 (display_initの後)
    // ui_events_init();     // UIイベントを関連付け (ui_initの後)
    
    uart_init(process_incoming_data); // UART受信開始、コールバックを登録

    // --- 起動後のUIセットアップ ---
    populate_log_list(); // ホーム画面のログ一覧を埋める
    
    // 設定に基づいて起動スクリーンをロード
    if (settings.startupScreen == "raw_mode") {
        lv_scr_load(ui_RawScreen);
    } else if (settings.startupScreen == "analytics_mode") {
        lv_scr_load(ui_AnalyticsScreen);
    } else { // "home" or default
        lv_scr_load(ui_HomeScreen);
    }


    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.println( LVGL_Arduino );
    Serial.println( "I am LVGL_Arduino" );

    lv_init();

#if LV_USE_LOG != 0
    lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

    ui_init();
    // Light
    // lv_obj_set_style_text_font(ui_LabelHome1, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_label_set_text( ui_LabelHome1, LV_SYMBOL_NEW_LINE);
    // // A/C
    // lv_obj_set_style_text_font(ui_LabelHome2, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_label_set_text( ui_LabelHome2, LV_SYMBOL_NEW_LINE);
    // lv_label_set_text( ui_LabelTempUp, LV_SYMBOL_UP);
    // lv_label_set_text( ui_LabelTempDown, LV_SYMBOL_DOWN);
    // lv_obj_set_style_text_font(ui_LabelACOn, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_label_set_text( ui_LabelACOn, LV_SYMBOL_POWER);
    // lv_obj_set_style_text_font(ui_LabelACOff, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
    // // lv_label_set_text( ui_LabelACOff, LV_SYMBOL_CLOSE);
    // lv_label_set_text( ui_LabelACOff, "OFF");
    // // Curtain
    // lv_obj_set_style_text_font(ui_LabelHome3, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_label_set_text( ui_LabelHome3, LV_SYMBOL_NEW_LINE);
    // lv_obj_set_style_text_font(ui_LabelOpen, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_label_set_text( ui_LabelOpen, LV_SYMBOL_LEFT LV_SYMBOL_RIGHT);
    // lv_obj_set_style_text_font(ui_LabelStop, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_label_set_text( ui_LabelStop, LV_SYMBOL_PAUSE);
    // lv_obj_set_style_text_font(ui_LabelClose, &lv_font_montserrat_26, LV_PART_MAIN | LV_STATE_DEFAULT);
    // lv_label_set_text( ui_LabelClose, LV_SYMBOL_RIGHT LV_SYMBOL_LEFT);

    Serial.println( "Setup done" );
}

void loop()
{
    // lv_timer_handler(); /* let the GUI do its work */

    // if (SerialPort.available()) {
    //     String receivedData = SerialPort.readStringUntil('\n');
    //     processUartData(receivedData);
    // }

    // sprintf(chbuff, "%02d:%02d", tm->tm_hour, tm->tm_min);
    // lv_label_set_text(uic_sample, chbuff);
    // lv_label_set_text(uic_aa, chbuff);
    // lv_label_get

    // delay(5);
    display_handler();
    uart_handler();
}

