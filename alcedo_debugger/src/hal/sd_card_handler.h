/**
 * @file sd_card_handler.h
 * @brief SDカードの初期化、ファイルの読み書きなど、SDカード関連の処理をすべて担当します。
 */
#pragma once
#include <Arduino.h>

// static String currentLogFile;
extern String currentLogFile;

/**
 * @brief SDカードの初期化とマウントを行います。
 * @details 起動時に一度だけ呼び出してください。失敗した場合はUIにエラーを表示します。
 */
void sd_card_init();

/**
 * @brief 新しいログファイルを作成（または次のログファイルを準備）します。
 * @details 起動時やインデックス行受信時に呼び出され、
 * log_001.csv, log_002.csv のように連番のファイル名を決定します。
 */
void create_new_log_file();

/**
 * @brief 現在のログファイルにタイムスタンプ付きでデータを追記します。
 * @param data 書き込む文字列データ。
 */
void log_to_sd(const String& data);

/**
 * @brief SDカード内のログファイルをスキャンし、ホーム画面のリストに表示します。
 */
void populate_log_list();

/**
 * @brief 指定されたログファイルの内容を読み込みます。
 * @param filename 読み込むファイル名 (例: "/log_001.csv")
 * @return String ファイルの内容（最大50行）。ファイルが開けない場合はエラーメッセージを返します。
 */
String read_log_file_content(const String& filename);