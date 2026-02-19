// Example: UART echo – reads from UART1, echoes back
// Demonstrates ESP-IDF UART driver mock
#include <Arduino.h>
#include "driver/uart.h"
#include <cstdio>
#include <cstring>

void setup() {
    uart_config_t cfg = {};
    cfg.baud_rate = 115200;
    cfg.data_bits = UART_DATA_8_BITS;
    cfg.parity = UART_PARITY_DISABLE;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;

    uart_param_config(UART_NUM_1, &cfg);
    uart_set_pin(UART_NUM_1, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM_1, 1024, 0, 0, nullptr, 0);

    printf("[uart_echo] UART1 configured at 115200 baud\n");

    // Inject some test data to simulate incoming bytes
    const char* testMsg = "Hello ESP32!";
    esp32emu::UartDriver::instance().port(UART_NUM_1).injectRx(
        (const uint8_t*)testMsg, strlen(testMsg));
}

void loop() {
    uint8_t buf[128];
    int len = uart_read_bytes(UART_NUM_1, buf, sizeof(buf) - 1, 100);
    if (len > 0) {
        buf[len] = '\0';
        printf("[uart_echo] Received %d bytes: %s\n", len, (char*)buf);
        // Echo back
        uart_write_bytes(UART_NUM_1, buf, len);
        printf("[uart_echo] Echoed %d bytes\n", len);
    }
}
