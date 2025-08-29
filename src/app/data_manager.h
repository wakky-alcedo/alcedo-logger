/**
 * @file data_manager.h
 * @brief 受信したUARTデータの解析と、それに応じたUI更新の司令塔となるモジュールです。
 */
#pragma once
#include <Arduino.h>

/**
 * @brief UARTハンドラから呼び出されるメインのデータ処理関数。
 * @details 受信データをログに保存し、内容を解析して各UI画面の表示を更新します。
 * @param data UARTから受信した1行分のデータ。
 */
void process_incoming_data(const String& data);