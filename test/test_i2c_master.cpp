#include "driver/i2c_master.h"
#include <cassert>
#include <cstdio>

int main() {
    // Create bus
    i2c_master_bus_config_t bus_cfg = {};
    bus_cfg.i2c_port = 0;
    bus_cfg.sda_io_num = 21;
    bus_cfg.scl_io_num = 22;
    bus_cfg.clk_source = I2C_CLK_SRC_DEFAULT;

    i2c_master_bus_handle_t bus = nullptr;
    assert(i2c_new_master_bus(&bus_cfg, &bus) == ESP_OK);
    assert(bus != nullptr);

    // Add device
    i2c_device_config_t dev_cfg = {};
    dev_cfg.device_address = 0x68;
    dev_cfg.scl_speed_hz = 400000;

    i2c_master_dev_handle_t dev = nullptr;
    assert(i2c_master_bus_add_device(bus, &dev_cfg, &dev) == ESP_OK);
    assert(dev != nullptr);

    // Set test response
    uint8_t response[] = {0x12, 0x34, 0x56};
    i2c_master_bus_test_set_response(bus, 0x68, response, sizeof(response));

    // Probe
    assert(i2c_master_probe(bus, 0x68, 100) == ESP_OK);
    assert(i2c_master_probe(bus, 0x50, 100) == ESP_ERR_NOT_FOUND);

    // Transmit
    uint8_t tx[] = {0x00, 0x01};
    assert(i2c_master_transmit(dev, tx, sizeof(tx), 100) == ESP_OK);
    auto written = i2c_master_bus_test_get_written(bus, 0x68);
    assert(written.size() == 2);
    assert(written[0] == 0x00);
    assert(written[1] == 0x01);

    // Receive
    uint8_t rx[3] = {};
    assert(i2c_master_receive(dev, rx, 3, 100) == ESP_OK);
    assert(rx[0] == 0x12);
    assert(rx[1] == 0x34);
    assert(rx[2] == 0x56);

    // Transmit-receive
    i2c_master_bus_test_set_response(bus, 0x68, response, sizeof(response));
    uint8_t reg = 0x0F;
    uint8_t rx2[2] = {};
    assert(i2c_master_transmit_receive(dev, &reg, 1, rx2, 2, 100) == ESP_OK);
    assert(rx2[0] == 0x12);
    assert(rx2[1] == 0x34);

    // Cleanup
    assert(i2c_master_bus_rm_device(dev) == ESP_OK);
    assert(i2c_del_master_bus(bus) == ESP_OK);

    printf("test_i2c_master: all assertions passed\n");
    return 0;
}
