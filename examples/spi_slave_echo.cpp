// SPI slave echo example — receives data from master and echoes back
#include <Arduino.h>
#include "driver/spi_slave.h"

void setup() {
    Serial.begin(115200);
    Serial.println("SPI Slave Echo");

    spi_bus_config_t bus = {};
    bus.mosi_io_num = 23;
    bus.miso_io_num = 19;
    bus.sclk_io_num = 18;
    bus.quadwp_io_num = -1;
    bus.quadhd_io_num = -1;

    spi_slave_interface_config_t cfg = {};
    cfg.spics_io_num = 5;
    cfg.queue_size = 3;
    cfg.mode = 0;

    spi_slave_initialize(SPI2_HOST, &bus, &cfg, 0);

    // Inject test data
    uint8_t test[] = {0xDE, 0xAD, 0xBE, 0xEF};
    esp32emu_spi_slave::injectRxData(test, 4);

    uint8_t tx[4] = {0x01, 0x02, 0x03, 0x04};
    uint8_t rx[4] = {0};
    spi_slave_transaction_t trans = {};
    trans.length = 32;
    trans.tx_buffer = tx;
    trans.rx_buffer = rx;

    spi_slave_transmit(SPI2_HOST, &trans, 1000);
    Serial.print("Received: ");
    for (int i = 0; i < 4; i++) {
        Serial.print("0x"); Serial.print(rx[i], HEX); Serial.print(" ");
    }
    Serial.println();

    spi_slave_free(SPI2_HOST);
}

void loop() {
    delay(1000);
}
