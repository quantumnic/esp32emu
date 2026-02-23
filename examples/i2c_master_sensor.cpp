// ESP-IDF v5 I2C master driver — read sensor register
#include <Arduino.h>
#include <driver/i2c_master.h>

void setup() {
    Serial.begin(115200);

    i2c_master_bus_config_t bus_cfg = {};
    bus_cfg.i2c_port = 0;
    bus_cfg.sda_io_num = 21;
    bus_cfg.scl_io_num = 22;
    bus_cfg.clk_source = I2C_CLK_SRC_DEFAULT;

    i2c_master_bus_handle_t bus;
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &bus));

    i2c_device_config_t dev_cfg = {};
    dev_cfg.device_address = 0x68;
    dev_cfg.scl_speed_hz = 400000;

    i2c_master_dev_handle_t sensor;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus, &dev_cfg, &sensor));

    // Read WHO_AM_I register
    uint8_t reg = 0x75;
    uint8_t who_am_i = 0;
    ESP_ERROR_CHECK(i2c_master_transmit_receive(sensor, &reg, 1, &who_am_i, 1, 100));
    Serial.print("WHO_AM_I: 0x");
    Serial.println(who_am_i, HEX);

    i2c_master_bus_rm_device(sensor);
    i2c_del_master_bus(bus);
}

void loop() {
    delay(1000);
}
