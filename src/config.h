/**
 * @file config.h
 * @brief プロジェクト全体のハードウェアピン設定や定数を定義します。
 */
#pragma once

// --- Hardware Pin Definitions ---
// TODO: ご自身の環境に合わせてピン番号を変更してください
#define SD_CS_PIN 14  ///< SDカードのCS (Chip Select) ピン

// LovyanGFXのヘッダをインクルード（ここでピン設定を行うため）
#include <LGFX_Config.h> // ★ このファイルは別途作成が必要です

// --- UART Settings ---
#define UART_INSTANCE 2    ///< 使用するUARTの番号 (UART2)
#define UART_RX_PIN 16     ///< UART2の受信(RX)ピン
#define UART_TX_PIN 17     ///< UART2の送信(TX)ピン
#define UART_BAUDRATE 115200 ///< UARTのボーレート

// --- Application Settings ---
#define LOG_FILE_PREFIX "/log_"            ///< ログファイルの接頭辞
#define LOG_FILE_EXTENSION ".csv"          ///< ログファイルの拡張子
#define SETTINGS_FILE "/settings.json"     ///< 設定ファイル名