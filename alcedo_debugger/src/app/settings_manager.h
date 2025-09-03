/**
 * @file settings_manager.h
 * @brief アプリケーションの設定を管理し、SDカードのJSONファイルへの保存・読み込みを行います。
 */
#pragma once
#include <Arduino.h>

/**
 * @struct AppSettings
 * @brief アプリケーションの設定項目を保持する構造体。
 */
struct AppSettings {
    String startupScreen = "home"; ///< @var 起動時に表示する画面 (home, raw_mode, analytics_mode)
    int baudRate = 115200;         ///< @var シリアル通信のボーレート
};

/**
 * @brief アプリケーション全体で共有される設定情報。
 */
extern AppSettings settings;

/**
 * @brief 設定を初期化します。
 * @details SDカードから `settings.json` を読み込み、`settings`構造体に格納します。
 * ファイルが存在しない場合は、デフォルト値で新規作成します。
 */
void settings_init();

/**
 * @brief 現在の`settings`構造体の内容をSDカードのJSONファイルに保存します。
 */
void save_settings();