// Test: ESP-IDF UART driver mock
#include "driver/uart.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Configure UART1
    uart_config_t cfg = {};
    cfg.baud_rate = 115200;
    cfg.data_bits = UART_DATA_8_BITS;
    cfg.parity = UART_PARITY_DISABLE;
    cfg.stop_bits = UART_STOP_BITS_1;
    cfg.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;

    assert(uart_param_config(UART_NUM_1, &cfg) == ESP_OK);
    assert(uart_set_pin(UART_NUM_1, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) == ESP_OK);
    assert(uart_driver_install(UART_NUM_1, 1024, 0, 0, nullptr, 0) == ESP_OK);

    // Write data
    const char* msg = "Hello UART";
    int written = uart_write_bytes(UART_NUM_1, msg, strlen(msg));
    assert(written == (int)strlen(msg));

    // Verify TX data
    auto txData = esp32emu::UartDriver::instance().port(UART_NUM_1).getTxData();
    assert(txData.size() == strlen(msg));
    assert(memcmp(txData.data(), msg, strlen(msg)) == 0);

    // Inject and read RX data
    const uint8_t rxIn[] = {0x01, 0x02, 0x03, 0x04};
    esp32emu::UartDriver::instance().port(UART_NUM_1).injectRx(rxIn, 4);

    size_t avail = 0;
    assert(uart_get_buffered_data_len(UART_NUM_1, &avail) == ESP_OK);
    assert(avail == 4);

    uint8_t buf[8];
    int nread = uart_read_bytes(UART_NUM_1, buf, sizeof(buf), 100);
    assert(nread == 4);
    assert(buf[0] == 0x01 && buf[3] == 0x04);

    // After read, buffer should be empty
    assert(uart_get_buffered_data_len(UART_NUM_1, &avail) == ESP_OK);
    assert(avail == 0);

    // Partial read
    esp32emu::UartDriver::instance().port(UART_NUM_1).injectRx(rxIn, 4);
    nread = uart_read_bytes(UART_NUM_1, buf, 2, 100);
    assert(nread == 2);
    assert(buf[0] == 0x01 && buf[1] == 0x02);
    nread = uart_read_bytes(UART_NUM_1, buf, 2, 100);
    assert(nread == 2);
    assert(buf[0] == 0x03 && buf[1] == 0x04);

    // Cleanup
    assert(uart_driver_delete(UART_NUM_1) == ESP_OK);

    printf("test_uart: all assertions passed\n");
    return 0;
}
