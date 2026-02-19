#include "driver/i2c.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Configure I2C master
    i2c_config_t conf{};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = 21;
    conf.scl_io_num = 22;
    conf.sda_pullup_en = true;
    conf.scl_pullup_en = true;
    conf.master.clk_speed = 400000;

    assert(i2c_param_config(I2C_NUM_0, &conf) == ESP_OK);
    assert(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0) == ESP_OK);

    // Register a mock BME280 at address 0x76
    esp32emu::i2c::registerSlave(0x76, [](uint8_t addr, const std::vector<uint8_t>& wd) -> std::vector<uint8_t> {
        (void)addr;
        // If register read for chip ID (0xD0), return 0x60
        if (!wd.empty() && wd[0] == 0xD0) {
            return {0x60};
        }
        // Temperature register (0xFA): return some bytes
        if (!wd.empty() && wd[0] == 0xFA) {
            return {0x80, 0x00, 0x00};
        }
        return {0xFF};
    });

    // Read chip ID
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (0x76 << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
        uint8_t reg = 0xD0;
        i2c_master_write(cmd, &reg, 1, ACK_CHECK_EN);
        i2c_master_start(cmd);  // repeated start
        i2c_master_write_byte(cmd, (0x76 << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
        uint8_t id = 0;
        i2c_master_read_byte(cmd, &id, I2C_MASTER_NACK);
        i2c_master_stop(cmd);
        assert(i2c_master_cmd_begin(I2C_NUM_0, cmd, 100) == ESP_OK);
        i2c_cmd_link_delete(cmd);
        assert(id == 0x60);
    }

    // Read temperature (3 bytes)
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (0x76 << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
        uint8_t reg = 0xFA;
        i2c_master_write(cmd, &reg, 1, ACK_CHECK_EN);
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (0x76 << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
        uint8_t buf[3] = {};
        i2c_master_read(cmd, buf, 3, I2C_MASTER_LAST_NACK);
        i2c_master_stop(cmd);
        assert(i2c_master_cmd_begin(I2C_NUM_0, cmd, 100) == ESP_OK);
        i2c_cmd_link_delete(cmd);
        assert(buf[0] == 0x80);
        assert(buf[1] == 0x00);
        assert(buf[2] == 0x00);
    }

    // Uninstall driver
    assert(i2c_driver_delete(I2C_NUM_0) == ESP_OK);

    // cmd_begin should fail without driver
    {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_stop(cmd);
        assert(i2c_master_cmd_begin(I2C_NUM_0, cmd, 100) == ESP_FAIL);
        i2c_cmd_link_delete(cmd);
    }

    esp32emu::i2c::clearSlaves();

    printf("test_i2c_driver: all assertions passed\n");
    return 0;
}
