/**
 * @file uart_handler.h
 * @brief マイコン間でのUART通信の受信処理を担当します。
 */
#pragma once
#include <Arduino.h>

/**
 * @brief UARTを指定した設定で初期化します。
 * @param callback データを受信したときに呼び出されるコールバック関数へのポインタ。
 * 受信した1行分のデータが引数として渡されます。
 */
void uart_init(void (*callback)(const String&));

/**
 * @brief UARTの受信バッファをチェックし、データがあればコールバックを呼び出します。
 * @details この関数をメインループ内で定期的に呼び出す必要があります。
 */
void uart_handler();