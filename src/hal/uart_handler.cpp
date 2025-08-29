#include "uart_handler.h"
#include "../config.h"
#include <HardwareSerial.h>

static HardwareSerial SerialPort(UART_INSTANCE);
static void (*data_received_callback)(const String&) = nullptr;

void uart_init(void (*callback)(const String&)) {
    SerialPort.begin(UART_BAUDRATE, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);
    data_received_callback = callback;
}

void uart_handler() {
    if (SerialPort.available()) {
        String data = SerialPort.readStringUntil('\n');
        data.trim();
        if (data.length() > 0 && data_received_callback != nullptr) {
            data_received_callback(data);
        }
    }
}