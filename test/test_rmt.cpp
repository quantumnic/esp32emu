// test_rmt: ESP32 RMT (Remote Control Transceiver) driver mock tests
#include "driver/rmt.h"
#include <cassert>
#include <cstdio>

int main() {
    rmt_test::reset();

    // Configure TX channel
    rmt_config_t tx_cfg{};
    tx_cfg.rmt_mode = RMT_MODE_TX;
    tx_cfg.channel = RMT_CHANNEL_0;
    tx_cfg.gpio_num = 18;
    tx_cfg.clk_div = 80; // 1us resolution at 80MHz
    tx_cfg.mem_block_num = 1;
    tx_cfg.tx_config.carrier_en = false;
    tx_cfg.tx_config.idle_output_en = true;
    tx_cfg.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;

    assert(rmt_config(&tx_cfg) == ESP_OK);

    // Install driver
    assert(rmt_driver_install(RMT_CHANNEL_0, 0, 0) == ESP_OK);
    assert(rmt_driver_install(RMT_CHANNEL_0, 0, 0) == ESP_ERR_INVALID_STATE); // double install

    // Write items (simulate WS2812B-like timing)
    rmt_item32_t items[3];
    items[0].level0 = 1; items[0].duration0 = 400;
    items[0].level1 = 0; items[0].duration1 = 850;
    items[1].level0 = 1; items[1].duration0 = 800;
    items[1].level1 = 0; items[1].duration1 = 450;
    items[2].val = 0; // end marker

    assert(rmt_write_items(RMT_CHANNEL_0, items, 3, true) == ESP_OK);
    assert(rmt_test::getTxCount(RMT_CHANNEL_0) == 1);
    assert(rmt_test::getTxLog(RMT_CHANNEL_0).size() == 3);
    assert(rmt_test::getTxLog(RMT_CHANNEL_0)[0].duration0 == 400);
    assert(rmt_test::getTxLog(RMT_CHANNEL_0)[1].duration0 == 800);

    // Wait TX done
    assert(rmt_wait_tx_done(RMT_CHANNEL_0, 1000) == ESP_OK);

    // Second write appends to log
    assert(rmt_write_items(RMT_CHANNEL_0, items, 2, true) == ESP_OK);
    assert(rmt_test::getTxCount(RMT_CHANNEL_0) == 2);
    assert(rmt_test::getTxLog(RMT_CHANNEL_0).size() == 5);

    // Configure RX channel
    rmt_config_t rx_cfg{};
    rx_cfg.rmt_mode = RMT_MODE_RX;
    rx_cfg.channel = RMT_CHANNEL_1;
    rx_cfg.gpio_num = 19;
    rx_cfg.clk_div = 80;
    rx_cfg.mem_block_num = 1;
    rx_cfg.rx_config.filter_en = true;
    rx_cfg.rx_config.filter_ticks_thresh = 100;
    rx_cfg.rx_config.idle_threshold = 12000;

    assert(rmt_config(&rx_cfg) == ESP_OK);
    assert(rmt_driver_install(RMT_CHANNEL_1, 1000, 0) == ESP_OK);

    // Inject RX items
    rmt_item32_t rx_items[2];
    rx_items[0].level0 = 1; rx_items[0].duration0 = 500;
    rx_items[0].level1 = 0; rx_items[0].duration1 = 500;
    rx_items[1].val = 0;
    rmt_test::injectRxItems(RMT_CHANNEL_1, rx_items, 2);

    // Invalid channel
    assert(rmt_config(nullptr) == ESP_ERR_INVALID_ARG);

    // Uninstall
    assert(rmt_driver_uninstall(RMT_CHANNEL_0) == ESP_OK);
    assert(rmt_driver_uninstall(RMT_CHANNEL_0) == ESP_ERR_INVALID_STATE); // not installed

    // Can't write after uninstall
    assert(rmt_write_items(RMT_CHANNEL_0, items, 1, true) == ESP_ERR_INVALID_STATE);

    // Cleanup
    rmt_test::reset();

    printf("test_rmt: all assertions passed\n");
    return 0;
}
