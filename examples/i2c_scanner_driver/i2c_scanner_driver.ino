// ESP-IDF I2C driver scanner example
// Scans the I2C bus using the low-level driver API

#include <Arduino.h>
#include "driver/i2c.h"

#define I2C_MASTER_SDA 21
#define I2C_MASTER_SCL 22
#define I2C_MASTER_FREQ 100000

void setup() {
    Serial.begin(115200);
    Serial.println("=== I2C Scanner (ESP-IDF driver) ===");

    i2c_config_t conf{};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA;
    conf.scl_io_num = I2C_MASTER_SCL;
    conf.sda_pullup_en = true;
    conf.scl_pullup_en = true;
    conf.master.clk_speed = I2C_MASTER_FREQ;

    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

    // Register a few mock devices for demo
    esp32emu::i2c::registerSlave(0x3C, [](uint8_t, const std::vector<uint8_t>&) {
        return std::vector<uint8_t>{0x00}; // SSD1306 OLED
    });
    esp32emu::i2c::registerSlave(0x68, [](uint8_t, const std::vector<uint8_t>&) {
        return std::vector<uint8_t>{0x68}; // MPU6050
    });
    esp32emu::i2c::registerSlave(0x76, [](uint8_t, const std::vector<uint8_t>&) {
        return std::vector<uint8_t>{0x60}; // BME280
    });

    Serial.println("Scanning...\n");
    Serial.println("     0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");

    int found = 0;
    for (int row = 0; row < 8; row++) {
        Serial.print("0x");
        Serial.print(row, HEX);
        Serial.print("0 ");
        for (int col = 0; col < 16; col++) {
            uint8_t addr = row * 16 + col;
            if (addr < 0x03 || addr > 0x77) {
                Serial.print("   ");
                continue;
            }

            i2c_cmd_handle_t cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
            i2c_master_stop(cmd);

            // Check if slave is registered (responds)
            auto it = esp32emu::i2c::slaves().find(addr);
            if (it != esp32emu::i2c::slaves().end()) {
                Serial.print(" ");
                if (addr < 0x10) Serial.print("0");
                Serial.print(addr, HEX);
                found++;
            } else {
                Serial.print(" --");
            }

            i2c_cmd_link_delete(cmd);
        }
        Serial.println();
    }

    Serial.print("\nFound ");
    Serial.print(found);
    Serial.println(" device(s)");

    i2c_driver_delete(I2C_NUM_0);
    esp32emu::i2c::clearSlaves();
}

void loop() {
    delay(5000);
}
