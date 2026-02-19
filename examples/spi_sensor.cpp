// Example: SPI sensor read – simulates reading a sensor via SPI
// Demonstrates ESP-IDF SPI master driver mock with responder
#include <Arduino.h>
#include "driver/spi_master.h"
#include <cstdio>

static spi_device_handle_t sensorDev;

void setup() {
    spi_bus_config_t busCfg = {};
    busCfg.mosi_io_num = 23;
    busCfg.miso_io_num = 19;
    busCfg.sclk_io_num = 18;
    busCfg.quadwp_io_num = -1;
    busCfg.quadhd_io_num = -1;

    spi_bus_initialize(VSPI_HOST, &busCfg, 1);

    spi_device_interface_config_t devCfg = {};
    devCfg.clock_speed_hz = 1000000;
    devCfg.spics_io_num = 5;
    devCfg.queue_size = 1;

    spi_bus_add_device(VSPI_HOST, &devCfg, &sensorDev);

    // Simulate a temperature sensor: command 0x01 returns temp high/low
    esp32emu::SpiMasterDriver::instance().setResponder(sensorDev,
        [](const uint8_t* tx, size_t len) -> std::vector<uint8_t> {
            std::vector<uint8_t> rx(len, 0);
            if (len >= 1 && tx[0] == 0x01) {
                // Return 25.5°C as raw: 0x0199 (high=0x01, low=0x99)
                if (len >= 2) rx[1] = 0x01;
                if (len >= 3) rx[2] = 0x99;
            }
            return rx;
        });

    printf("[spi_sensor] SPI bus initialized, sensor attached\n");
}

void loop() {
    uint8_t txBuf[4] = {0x01, 0x00, 0x00, 0x00}; // Read temp command
    uint8_t rxBuf[4] = {};

    spi_transaction_t trans = {};
    trans.length = 32;
    trans.tx_buffer = txBuf;
    trans.rx_buffer = rxBuf;

    spi_device_transmit(sensorDev, &trans);

    int16_t raw = ((int16_t)rxBuf[1] << 8) | rxBuf[2];
    float temp = raw / 10.0f;
    printf("[spi_sensor] Temperature: %.1f°C (raw: 0x%02X%02X)\n",
           temp, rxBuf[1], rxBuf[2]);
}
