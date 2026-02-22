#include "driver_twai.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    esp32emu_twai::reset();

    // Install and start
    twai_general_config_t g = TWAI_GENERAL_CONFIG_DEFAULT(21, 22, TWAI_MODE_NORMAL);
    twai_timing_config_t t = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    assert(twai_driver_install(&g, &t, &f) == ESP_OK);
    assert(twai_start() == ESP_OK);
    assert(esp32emu_twai::_state == TWAI_STATE_RUNNING);

    // Double start fails
    assert(twai_start() == ESP_ERR_INVALID_STATE);

    // Transmit
    twai_message_t tx_msg = {};
    tx_msg.identifier = 0x123;
    tx_msg.data_length_code = 4;
    tx_msg.data[0] = 0xDE; tx_msg.data[1] = 0xAD;
    tx_msg.data[2] = 0xBE; tx_msg.data[3] = 0xEF;
    assert(twai_transmit(&tx_msg, 100) == ESP_OK);
    assert(esp32emu_twai::_tx_queue.size() == 1);
    assert(esp32emu_twai::_tx_queue[0].identifier == 0x123);

    // Receive empty = timeout
    twai_message_t rx_msg;
    assert(twai_receive(&rx_msg, 0) == ESP_ERR_TIMEOUT);

    // Inject and receive
    twai_message_t injected = {};
    injected.identifier = 0x456;
    injected.data_length_code = 2;
    injected.data[0] = 0xCA; injected.data[1] = 0xFE;
    esp32emu_twai::injectRxMessage(injected);
    assert(twai_receive(&rx_msg, 100) == ESP_OK);
    assert(rx_msg.identifier == 0x456);
    assert(rx_msg.data[0] == 0xCA);

    // Status
    twai_status_info_t status;
    twai_get_status_info(&status);
    assert(status.state == TWAI_STATE_RUNNING);

    // Stop and uninstall
    assert(twai_stop() == ESP_OK);
    assert(twai_driver_uninstall() == ESP_OK);

    esp32emu_twai::reset();
    printf("test_twai: all assertions passed\n");
    return 0;
}
