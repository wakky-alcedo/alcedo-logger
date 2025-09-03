/**
 * @file uart_handler.cpp
 * @brief UARTドライバをイベントキューを使用した割り込み方式で実装します。(修正版)
 */
#include "uart_handler.h"
#include "config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include <lvgl.h>
#include "ui/ui.h" // UIオブジェクトを直接操作するため

// ★ UARTバッファサイズを小さくしてメモリ使用量を削減
#define UART_BUF_SIZE 256

// --- 静的変数 ---
static QueueHandle_t uart_queue;        // UARTドライバからのイベントを受け取るキュー
static QueueHandle_t g_uart_data_queue; // 受信データを処理するためのキュー
static void (*data_received_callback)(const String&) = nullptr; // データ処理用のコールバック関数

/**
 * @brief UARTイベントを処理するための専用タスク (バッファオーバーフロー対策版)
 * @details FreeRTOSのタスクとして裏側で常に動き続けます。
 * UARTキューにイベントが来るまで待機し、イベントが来たら処理を実行します。
 * @param pvParameters タスク生成時に渡されるパラメータ (今回は未使用)
 */
static void uart_event_task(void *pvParameters) {
    uart_event_t event;
    uint8_t* dtmp = (uint8_t*) malloc(UART_BUF_SIZE + 1);

    while (true) {
        if (xQueueReceive(uart_queue, (void *)&event, portMAX_DELAY)) {
            if (event.type == UART_DATA) {
                int read_len = uart_read_bytes(UART_INSTANCE, dtmp, event.size, portMAX_DELAY);
                if (read_len > 0) {
                    dtmp[read_len] = '\0';
                    
                    // ★★★ ここからが変更点 ★★★
                    // 受信したデータをコピーするための新しいメモリを確保
                    char* data_copy = (char*)malloc(read_len + 1);
                    if (data_copy != NULL) {
                        memcpy(data_copy, dtmp, read_len + 1);
                        // メモリのポインタをキューに送信
                        if (xQueueSend(g_uart_data_queue, &data_copy, (TickType_t)0) != pdPASS) {
                            // キューが満杯ならメモリを解放
                            free(data_copy);
                        }
                    }
                }
            } else if (event.type == UART_FIFO_OVF || event.type == UART_BUFFER_FULL) {
                uart_flush_input(UART_INSTANCE);
                xQueueReset(uart_queue);
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}

/**
 * @brief UARTドライバを割り込みモードで初期化し、受信タスクを開始します。
 * @param callback データを受信したときに呼び出されるコールバック関数へのポインタ。
 */
void uart_init(void (*callback)(const String&)) {
    // シリアル通信を強制的に終了してリソースを解放
    Serial.end();

    data_received_callback = callback;

    // 1. UARTの基本設定
    uart_config_t uart_config = {
        .baud_rate = settings.baudRate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    
    g_uart_data_queue = xQueueCreate(10, sizeof(char*));
    
    // キューの作成に失敗したかどうかをチェック
    if (g_uart_data_queue == NULL) {
        // もしデバッグ用に別のUARTが使えるなら、そこでエラーを出力
        // 使えない場合は、ディスプレイにエラーを表示して停止する
        lv_textarea_add_text(ui_debugmonitor, "Failed to create UART queue\n");
        while(1) { delay(1000); } // 無限ループで停止
    }

    // ★ エラーチェックを追加
    esp_err_t err = uart_driver_install(UART_INSTANCE, UART_BUF_SIZE * 2, UART_BUF_SIZE * 2, 20, &uart_queue, 0);
    if (err != ESP_OK) {
        // ドライバのインストールに失敗した場合、シリアルモニタにエラーを出力して停止
        // Serial.printf("Failed to install UART driver, error code: %d\n", err);
        lv_textarea_add_text(ui_debugmonitor, "Failed to install UART driver\n");
        return; // タスクを開始せずに終了
    }

    uart_param_config(UART_INSTANCE, &uart_config);
    uart_set_pin(UART_INSTANCE, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // ドライバのインストールが成功した場合のみタスクを作成
    xTaskCreate(uart_event_task, "uart_event_task", 2048, NULL, 12, NULL);
}

void uart_handler() {
    char* received_data = NULL;
    // 待ち時間0でキューからデータを受信試行
    if (xQueueReceive(g_uart_data_queue, &received_data, (TickType_t)0) == pdPASS) {
        if (received_data != NULL) {
            // データがあれば、データ処理関数を呼び出す
            data_received_callback(String(received_data));
            // 処理が終わったら、確保されたメモリを必ず解放する
            free(received_data);
        }
    }
}