#pragma once
// esp32emu — ESP-IDF v5 I2C master driver mock

#include "esp_err.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <map>
#include <functional>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct i2c_master_bus_t* i2c_master_bus_handle_t;
typedef struct i2c_master_dev_t* i2c_master_dev_handle_t;

typedef enum {
    I2C_CLK_SRC_DEFAULT = 0,
} i2c_clock_source_t;

typedef struct {
    int i2c_port;
    int sda_io_num;
    int scl_io_num;
    i2c_clock_source_t clk_source;
    uint8_t glitch_ignore_cnt;
    int intr_priority;
    struct {
        uint32_t enable_internal_pullup : 1;
    } flags;
} i2c_master_bus_config_t;

typedef struct {
    uint16_t dev_addr_length;  // 7 or 10
    uint16_t device_address;
    uint32_t scl_speed_hz;
    uint32_t scl_wait_us;
    struct {
        uint32_t disable_ack_check : 1;
    } flags;
} i2c_device_config_t;

// Internal structures
struct i2c_master_dev_t {
    uint16_t address;
    uint32_t speed;
    i2c_master_bus_handle_t bus;
    std::vector<uint8_t> rx_buffer;
    size_t rx_pos;
};

struct i2c_master_bus_t {
    int port;
    int sda;
    int scl;
    std::vector<i2c_master_dev_handle_t> devices;
    // Test: register response data per address
    std::map<uint16_t, std::vector<uint8_t>> test_responses;
    std::map<uint16_t, std::vector<uint8_t>> test_written;
};

// Test helper: set response data for a device address
inline void i2c_master_bus_test_set_response(i2c_master_bus_handle_t bus,
                                              uint16_t addr,
                                              const uint8_t* data, size_t len) {
    bus->test_responses[addr].assign(data, data + len);
}

inline std::vector<uint8_t> i2c_master_bus_test_get_written(i2c_master_bus_handle_t bus,
                                                             uint16_t addr) {
    return bus->test_written[addr];
}

inline esp_err_t i2c_new_master_bus(const i2c_master_bus_config_t* config,
                                     i2c_master_bus_handle_t* ret_bus) {
    auto bus = new i2c_master_bus_t();
    bus->port = config->i2c_port;
    bus->sda = config->sda_io_num;
    bus->scl = config->scl_io_num;
    *ret_bus = bus;
    fprintf(stderr, "[esp32emu] I2C master bus created on port %d (SDA=%d, SCL=%d)\n",
            bus->port, bus->sda, bus->scl);
    return ESP_OK;
}

inline esp_err_t i2c_master_bus_add_device(i2c_master_bus_handle_t bus,
                                            const i2c_device_config_t* config,
                                            i2c_master_dev_handle_t* ret_dev) {
    auto dev = new i2c_master_dev_t();
    dev->address = config->device_address;
    dev->speed = config->scl_speed_hz;
    dev->bus = bus;
    dev->rx_pos = 0;
    bus->devices.push_back(dev);
    *ret_dev = dev;
    fprintf(stderr, "[esp32emu] I2C device added: addr=0x%02X speed=%uHz\n",
            dev->address, dev->speed);
    return ESP_OK;
}

inline esp_err_t i2c_master_transmit(i2c_master_dev_handle_t dev,
                                      const uint8_t* write_buffer,
                                      size_t write_size, int timeout_ms) {
    (void)timeout_ms;
    auto& written = dev->bus->test_written[dev->address];
    written.insert(written.end(), write_buffer, write_buffer + write_size);
    fprintf(stderr, "[esp32emu] I2C TX to 0x%02X: %zu bytes\n", dev->address, write_size);
    return ESP_OK;
}

inline esp_err_t i2c_master_receive(i2c_master_dev_handle_t dev,
                                     uint8_t* read_buffer,
                                     size_t read_size, int timeout_ms) {
    (void)timeout_ms;
    auto& resp = dev->bus->test_responses[dev->address];
    for (size_t i = 0; i < read_size; i++) {
        if (dev->rx_pos < resp.size()) {
            read_buffer[i] = resp[dev->rx_pos++];
        } else {
            read_buffer[i] = 0xFF;
        }
    }
    fprintf(stderr, "[esp32emu] I2C RX from 0x%02X: %zu bytes\n", dev->address, read_size);
    return ESP_OK;
}

inline esp_err_t i2c_master_transmit_receive(i2c_master_dev_handle_t dev,
                                              const uint8_t* write_buffer,
                                              size_t write_size,
                                              uint8_t* read_buffer,
                                              size_t read_size,
                                              int timeout_ms) {
    i2c_master_transmit(dev, write_buffer, write_size, timeout_ms);
    dev->rx_pos = 0; // Reset read position for response
    return i2c_master_receive(dev, read_buffer, read_size, timeout_ms);
}

inline esp_err_t i2c_master_bus_rm_device(i2c_master_dev_handle_t dev) {
    auto& devices = dev->bus->devices;
    for (auto it = devices.begin(); it != devices.end(); ++it) {
        if (*it == dev) { devices.erase(it); break; }
    }
    delete dev;
    return ESP_OK;
}

inline esp_err_t i2c_del_master_bus(i2c_master_bus_handle_t bus) {
    for (auto d : bus->devices) delete d;
    delete bus;
    return ESP_OK;
}

inline esp_err_t i2c_master_probe(i2c_master_bus_handle_t bus,
                                   uint16_t address, int timeout_ms) {
    (void)timeout_ms;
    // Probe succeeds if response data is registered
    if (bus->test_responses.count(address)) return ESP_OK;
    return ESP_ERR_NOT_FOUND;
}

#ifdef __cplusplus
}
#endif
