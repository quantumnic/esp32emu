#pragma once
#include <cstdint>
#include <cstring>
#include <vector>
#include <map>
#include <functional>

class TwoWire {
public:
    void begin(int sda = -1, int scl = -1) { (void)sda; (void)scl; }
    void end() {}
    void setClock(uint32_t freq) { (void)freq; }

    void beginTransmission(uint8_t addr) { tx_addr_ = addr; tx_buf_.clear(); }
    size_t write(uint8_t val) { tx_buf_.push_back(val); return 1; }
    size_t write(const uint8_t* buf, size_t len) {
        tx_buf_.insert(tx_buf_.end(), buf, buf + len);
        return len;
    }
    uint8_t endTransmission(bool stop = true) { (void)stop; return 0; } // 0 = success

    uint8_t requestFrom(uint8_t addr, uint8_t qty) {
        rx_buf_.clear();
        rx_pos_ = 0;
        auto it = devices_.find(addr);
        if (it != devices_.end()) {
            rx_buf_ = it->second(tx_buf_, qty);
        } else {
            rx_buf_.resize(qty, 0);
        }
        return (uint8_t)rx_buf_.size();
    }

    int available() { return (int)(rx_buf_.size() - rx_pos_); }
    int read() {
        if (rx_pos_ < rx_buf_.size()) return rx_buf_[rx_pos_++];
        return -1;
    }

    // Emulator-only: register mock I2C device
    using DeviceCallback = std::function<std::vector<uint8_t>(
        const std::vector<uint8_t>& tx_data, uint8_t requested_bytes)>;

    void addDevice(uint8_t addr, DeviceCallback cb) { devices_[addr] = cb; }
    void removeDevice(uint8_t addr) { devices_.erase(addr); }

private:
    uint8_t tx_addr_ = 0;
    std::vector<uint8_t> tx_buf_;
    std::vector<uint8_t> rx_buf_;
    size_t rx_pos_ = 0;
    std::map<uint8_t, DeviceCallback> devices_;
};

extern TwoWire Wire;
