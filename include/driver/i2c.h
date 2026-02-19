#pragma once
// esp32emu — ESP-IDF I2C driver mock
// Emulates i2c_param_config, i2c_driver_install, i2c_master_cmd_handle, read/write commands

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <map>
#include <functional>

typedef uint32_t TickType_t;
typedef int i2c_port_t;
typedef int i2c_mode_t;
typedef void* i2c_cmd_handle_t;
typedef int esp_err_t;

#ifndef ESP_OK
#define ESP_OK 0
#define ESP_FAIL -1
#define ESP_ERR_INVALID_ARG 0x102
#define ESP_ERR_TIMEOUT 0x107
#endif

#define I2C_NUM_0 0
#define I2C_NUM_1 1
#define I2C_MODE_MASTER 0
#define I2C_MODE_SLAVE 1
#define I2C_MASTER_WRITE 0
#define I2C_MASTER_READ 1
#define ACK_CHECK_EN 0x1
#define ACK_CHECK_DIS 0x0
#define I2C_MASTER_ACK 0
#define I2C_MASTER_NACK 1
#define I2C_MASTER_LAST_NACK 2

struct i2c_config_t {
    i2c_mode_t mode;
    int sda_io_num;
    int scl_io_num;
    bool sda_pullup_en;
    bool scl_pullup_en;
    union {
        struct { uint32_t clk_speed; } master;
        struct { uint8_t addr_10bit_en; uint16_t slave_addr; uint32_t maximum_speed; } slave;
    };
};

namespace esp32emu {
namespace i2c {

enum class CmdType { Start, Stop, WriteByte, WriteBuffer, ReadBuffer };

struct Cmd {
    CmdType type;
    uint8_t byte;
    uint8_t* buf;
    size_t len;
    int ack;
};

struct CmdLink {
    std::vector<Cmd> cmds;
};

struct PortState {
    bool installed = false;
    i2c_config_t config{};
};

// Test helper: register a slave response callback
// callback receives (address, write_data) and returns read_data
using SlaveCallback = std::function<std::vector<uint8_t>(uint8_t addr, const std::vector<uint8_t>& writeData)>;

inline std::map<i2c_port_t, PortState>& ports() {
    static std::map<i2c_port_t, PortState> p;
    return p;
}

inline std::map<uint8_t, SlaveCallback>& slaves() {
    static std::map<uint8_t, SlaveCallback> s;
    return s;
}

inline void registerSlave(uint8_t addr, SlaveCallback cb) {
    slaves()[addr] = std::move(cb);
}

inline void clearSlaves() { slaves().clear(); }

} // namespace i2c
} // namespace esp32emu

inline esp_err_t i2c_param_config(i2c_port_t port, const i2c_config_t* config) {
    if (!config) return ESP_ERR_INVALID_ARG;
    esp32emu::i2c::ports()[port].config = *config;
    fprintf(stderr, "[esp32emu] i2c_param_config: port=%d mode=%d sda=%d scl=%d\n",
            port, config->mode, config->sda_io_num, config->scl_io_num);
    return ESP_OK;
}

inline esp_err_t i2c_driver_install(i2c_port_t port, i2c_mode_t mode, size_t slv_rx_buf, size_t slv_tx_buf, int intr_alloc_flags) {
    (void)mode; (void)slv_rx_buf; (void)slv_tx_buf; (void)intr_alloc_flags;
    esp32emu::i2c::ports()[port].installed = true;
    fprintf(stderr, "[esp32emu] i2c_driver_install: port=%d\n", port);
    return ESP_OK;
}

inline esp_err_t i2c_driver_delete(i2c_port_t port) {
    esp32emu::i2c::ports()[port].installed = false;
    return ESP_OK;
}

inline i2c_cmd_handle_t i2c_cmd_link_create() {
    return (i2c_cmd_handle_t)(new esp32emu::i2c::CmdLink());
}

inline void i2c_cmd_link_delete(i2c_cmd_handle_t cmd_handle) {
    delete static_cast<esp32emu::i2c::CmdLink*>(cmd_handle);
}

inline esp_err_t i2c_master_start(i2c_cmd_handle_t cmd_handle) {
    auto* link = static_cast<esp32emu::i2c::CmdLink*>(cmd_handle);
    link->cmds.push_back({esp32emu::i2c::CmdType::Start, 0, nullptr, 0, 0});
    return ESP_OK;
}

inline esp_err_t i2c_master_stop(i2c_cmd_handle_t cmd_handle) {
    auto* link = static_cast<esp32emu::i2c::CmdLink*>(cmd_handle);
    link->cmds.push_back({esp32emu::i2c::CmdType::Stop, 0, nullptr, 0, 0});
    return ESP_OK;
}

inline esp_err_t i2c_master_write_byte(i2c_cmd_handle_t cmd_handle, uint8_t data, bool ack_en) {
    auto* link = static_cast<esp32emu::i2c::CmdLink*>(cmd_handle);
    link->cmds.push_back({esp32emu::i2c::CmdType::WriteByte, data, nullptr, 0, ack_en ? 1 : 0});
    return ESP_OK;
}

inline esp_err_t i2c_master_write(i2c_cmd_handle_t cmd_handle, const uint8_t* data, size_t data_len, bool ack_en) {
    auto* link = static_cast<esp32emu::i2c::CmdLink*>(cmd_handle);
    // Store copy — we need it at execution time
    link->cmds.push_back({esp32emu::i2c::CmdType::WriteBuffer, 0, const_cast<uint8_t*>(data), data_len, ack_en ? 1 : 0});
    return ESP_OK;
}

inline esp_err_t i2c_master_read(i2c_cmd_handle_t cmd_handle, uint8_t* data, size_t data_len, int ack) {
    auto* link = static_cast<esp32emu::i2c::CmdLink*>(cmd_handle);
    link->cmds.push_back({esp32emu::i2c::CmdType::ReadBuffer, 0, data, data_len, ack});
    return ESP_OK;
}

inline esp_err_t i2c_master_read_byte(i2c_cmd_handle_t cmd_handle, uint8_t* data, int ack) {
    return i2c_master_read(cmd_handle, data, 1, ack);
}

inline esp_err_t i2c_master_cmd_begin(i2c_port_t port, i2c_cmd_handle_t cmd_handle, TickType_t ticks_to_wait) {
    (void)ticks_to_wait;
    if (!esp32emu::i2c::ports()[port].installed) return ESP_FAIL;

    auto* link = static_cast<esp32emu::i2c::CmdLink*>(cmd_handle);
    uint8_t addr = 0;
    bool isRead = false;
    std::vector<uint8_t> writeData;

    for (auto& cmd : link->cmds) {
        switch (cmd.type) {
        case esp32emu::i2c::CmdType::Start:
            break;
        case esp32emu::i2c::CmdType::WriteByte:
            // First write byte after start is address+RW
            if (addr == 0) {
                addr = cmd.byte >> 1;
                isRead = (cmd.byte & 1) != 0;
            } else {
                writeData.push_back(cmd.byte);
            }
            break;
        case esp32emu::i2c::CmdType::WriteBuffer:
            for (size_t i = 0; i < cmd.len; i++)
                writeData.push_back(cmd.buf[i]);
            break;
        case esp32emu::i2c::CmdType::ReadBuffer: {
            auto it = esp32emu::i2c::slaves().find(addr);
            if (it != esp32emu::i2c::slaves().end()) {
                auto resp = it->second(addr, writeData);
                size_t n = std::min(cmd.len, resp.size());
                std::memcpy(cmd.buf, resp.data(), n);
            } else {
                std::memset(cmd.buf, 0xFF, cmd.len);
            }
            break;
        }
        case esp32emu::i2c::CmdType::Stop:
            // If write-only transaction, still invoke callback
            if (!isRead && addr != 0) {
                auto it = esp32emu::i2c::slaves().find(addr);
                if (it != esp32emu::i2c::slaves().end()) {
                    it->second(addr, writeData);
                }
            }
            break;
        }
    }

    fprintf(stderr, "[esp32emu] i2c_master_cmd_begin: port=%d addr=0x%02X %s %zu bytes\n",
            port, addr, isRead ? "READ" : "WRITE", isRead ? (size_t)0 : writeData.size());
    return ESP_OK;
}
