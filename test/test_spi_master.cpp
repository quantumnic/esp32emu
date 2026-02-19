// Test: ESP-IDF SPI master driver mock
#include "driver/spi_master.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Init bus
    spi_bus_config_t busCfg = {};
    busCfg.mosi_io_num = 23;
    busCfg.miso_io_num = 19;
    busCfg.sclk_io_num = 18;
    busCfg.quadwp_io_num = -1;
    busCfg.quadhd_io_num = -1;
    busCfg.max_transfer_sz = 4096;

    assert(spi_bus_initialize(VSPI_HOST, &busCfg, 1) == ESP_OK);

    // Add device
    spi_device_interface_config_t devCfg = {};
    devCfg.clock_speed_hz = 1000000;
    devCfg.spics_io_num = 5;
    devCfg.queue_size = 1;

    spi_device_handle_t dev;
    assert(spi_bus_add_device(VSPI_HOST, &devCfg, &dev) == ESP_OK);

    // Set up a responder: echo bytes XOR'd with 0xFF
    esp32emu::SpiMasterDriver::instance().setResponder(dev,
        [](const uint8_t* tx, size_t len) -> std::vector<uint8_t> {
            std::vector<uint8_t> rx(len);
            for (size_t i = 0; i < len; i++) rx[i] = tx[i] ^ 0xFF;
            return rx;
        });

    // Full-buffer transaction
    uint8_t txBuf[4] = {0x01, 0x02, 0x03, 0x04};
    uint8_t rxBuf[4] = {};
    spi_transaction_t trans = {};
    trans.length = 32; // 4 bytes
    trans.tx_buffer = txBuf;
    trans.rx_buffer = rxBuf;

    assert(spi_device_transmit(dev, &trans) == ESP_OK);
    assert(rxBuf[0] == 0xFE); // 0x01 ^ 0xFF
    assert(rxBuf[1] == 0xFD);
    assert(rxBuf[2] == 0xFC);
    assert(rxBuf[3] == 0xFB);

    // Transaction with tx_data/rx_data
    spi_transaction_t trans2 = {};
    trans2.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
    trans2.length = 16; // 2 bytes
    trans2.tx_data[0] = 0xAA;
    trans2.tx_data[1] = 0x55;

    assert(spi_device_transmit(dev, &trans2) == ESP_OK);
    assert(trans2.rx_data[0] == 0x55); // 0xAA ^ 0xFF
    assert(trans2.rx_data[1] == 0xAA); // 0x55 ^ 0xFF

    // Cleanup
    assert(spi_bus_remove_device(dev) == ESP_OK);
    assert(spi_bus_free(VSPI_HOST) == ESP_OK);

    printf("test_spi_master: all assertions passed\n");
    return 0;
}
