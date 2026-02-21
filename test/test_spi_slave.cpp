#include "driver/spi_slave.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    esp32emu_spi_slave::reset();

    // Init
    spi_bus_config_t bus = {.mosi_io_num=23, .miso_io_num=19, .sclk_io_num=18, .quadwp_io_num=-1, .quadhd_io_num=-1};
    spi_slave_interface_config_t cfg = {};
    cfg.spics_io_num = 5;
    cfg.queue_size = 3;
    cfg.mode = 0;

    assert(spi_slave_initialize(SPI2_HOST, &bus, &cfg, 0) == ESP_OK);

    // Transmit with injected RX data
    uint8_t inject[] = {0xAA, 0xBB, 0xCC, 0xDD};
    esp32emu_spi_slave::injectRxData(inject, 4);

    uint8_t tx[4] = {0x01, 0x02, 0x03, 0x04};
    uint8_t rx[4] = {0};
    spi_slave_transaction_t trans = {};
    trans.length = 32; // 4 bytes * 8 bits
    trans.tx_buffer = tx;
    trans.rx_buffer = rx;

    assert(spi_slave_transmit(SPI2_HOST, &trans, 1000) == ESP_OK);
    assert(trans.trans_len == 32);
    assert(rx[0] == 0xAA && rx[1] == 0xBB && rx[2] == 0xCC && rx[3] == 0xDD);
    assert(esp32emu_spi_slave::getTransactionCount() == 1);

    // Second transaction with no inject data
    memset(rx, 0, 4);
    spi_slave_transaction_t trans2 = {};
    trans2.length = 32;
    trans2.tx_buffer = tx;
    trans2.rx_buffer = rx;
    assert(spi_slave_transmit(SPI2_HOST, &trans2, 1000) == ESP_OK);
    assert(rx[0] == 0); // no injected data

    // Free
    assert(spi_slave_free(SPI2_HOST) == ESP_OK);

    printf("test_spi_slave: all assertions passed\n");
    return 0;
}
