#pragma once
#include <cstdint>
#include <vector>
#include <cstring>

// Simulated OneWire address (8 bytes: family + 6 serial + CRC)
struct OneWireAddress {
    uint8_t addr[8];
};

class OneWire {
public:
    OneWire() : pin_(0) {}
    explicit OneWire(uint8_t pin) : pin_(pin) {}
    void begin(uint8_t pin) { pin_ = pin; }

    // Search
    void reset_search() { search_idx_ = 0; }
    bool search(uint8_t* addr) {
        if (search_idx_ >= (int)devices_.size()) return false;
        std::memcpy(addr, devices_[search_idx_].addr, 8);
        search_idx_++;
        return true;
    }

    // Bus operations (emulated)
    uint8_t reset() { return devices_.empty() ? 0 : 1; }
    void select(const uint8_t* /*addr*/) {}
    void skip() {}
    void write(uint8_t /*v*/, uint8_t /*power*/ = 0) {}
    void write_bytes(const uint8_t* /*buf*/, uint16_t /*count*/, bool /*power*/ = false) {}
    uint8_t read() { return read_buf_.empty() ? 0xFF : pop(); }
    void read_bytes(uint8_t* buf, uint16_t count) {
        for (uint16_t i = 0; i < count; i++) buf[i] = read();
    }
    void depower() {}

    static uint8_t crc8(const uint8_t* addr, uint8_t len) {
        uint8_t crc = 0;
        for (uint8_t i = 0; i < len; i++) {
            uint8_t mix = addr[i] ^ crc;
            crc = 0;
            if (mix & 0x01) crc ^= 0x5E;
            if (mix & 0x02) crc ^= 0xBC;
            if (mix & 0x04) crc ^= 0x61;
            if (mix & 0x08) crc ^= 0xC2;
            if (mix & 0x10) crc ^= 0x9D;
            if (mix & 0x20) crc ^= 0x23;
            if (mix & 0x40) crc ^= 0x46;
            if (mix & 0x80) crc ^= 0x8C;
        }
        return crc;
    }

    // --- Test helpers ---
    void addDevice(const uint8_t addr[8]) {
        OneWireAddress a;
        std::memcpy(a.addr, addr, 8);
        devices_.push_back(a);
    }

    void injectReadData(const std::vector<uint8_t>& data) {
        read_buf_.insert(read_buf_.end(), data.begin(), data.end());
    }

private:
    uint8_t pop() {
        uint8_t v = read_buf_.front();
        read_buf_.erase(read_buf_.begin());
        return v;
    }

    uint8_t pin_;
    std::vector<OneWireAddress> devices_;
    int search_idx_ = 0;
    std::vector<uint8_t> read_buf_;
};
