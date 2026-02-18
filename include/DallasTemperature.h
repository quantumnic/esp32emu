#pragma once
#include "OneWire.h"
#include <map>
#include <cstring>

// DS18B20 family code
#define DS18B20_FAMILY 0x28

typedef uint8_t DeviceAddress[8];

class DallasTemperature {
public:
    DallasTemperature() : wire_(nullptr) {}
    explicit DallasTemperature(OneWire* wire) : wire_(wire) {}

    void setOneWire(OneWire* wire) { wire_ = wire; }

    void begin() {
        device_count_ = 0;
        if (!wire_) return;
        wire_->reset_search();
        uint8_t addr[8];
        while (wire_->search(addr)) {
            if (addr[0] == DS18B20_FAMILY) {
                DevAddr da;
                std::memcpy(da.addr, addr, 8);
                devices_.push_back(da);
                device_count_++;
            }
        }
    }

    uint8_t getDeviceCount() const { return device_count_; }

    bool getAddress(uint8_t* addr, uint8_t index) const {
        if (index >= device_count_) return false;
        std::memcpy(addr, devices_[index].addr, 8);
        return true;
    }

    void requestTemperatures() { requested_ = true; }

    void requestTemperaturesByAddress(const uint8_t* /*addr*/) { requested_ = true; }

    float getTempCByIndex(uint8_t index) const {
        if (index >= device_count_) return -127.0f;
        auto key = addrKey(devices_[index].addr);
        auto it = temps_.find(key);
        return it != temps_.end() ? it->second : 22.5f; // default 22.5°C
    }

    float getTempFByIndex(uint8_t index) const {
        return getTempCByIndex(index) * 9.0f / 5.0f + 32.0f;
    }

    float getTempC(const uint8_t* addr) const {
        auto key = addrKey(addr);
        auto it = temps_.find(key);
        return it != temps_.end() ? it->second : 22.5f;
    }

    float getTempF(const uint8_t* addr) const {
        return getTempC(addr) * 9.0f / 5.0f + 32.0f;
    }

    void setResolution(uint8_t res) { resolution_ = res; }
    void setResolution(const uint8_t* /*addr*/, uint8_t res, bool /*skip*/ = false) { resolution_ = res; }
    uint8_t getResolution() const { return resolution_; }

    void setWaitForConversion(bool wait) { wait_ = wait; }
    bool getWaitForConversion() const { return wait_; }

    // --- Test helpers ---
    void setTemperature(const uint8_t* addr, float tempC) {
        temps_[addrKey(addr)] = tempC;
    }

    void setTemperatureByIndex(uint8_t index, float tempC) {
        if (index < device_count_) {
            temps_[addrKey(devices_[index].addr)] = tempC;
        }
    }

private:
    struct DevAddr { uint8_t addr[8]; };

    static uint64_t addrKey(const uint8_t* addr) {
        uint64_t key = 0;
        std::memcpy(&key, addr, 8);
        return key;
    }

    OneWire* wire_;
    std::vector<DevAddr> devices_;
    std::map<uint64_t, float> temps_;
    uint8_t device_count_ = 0;
    uint8_t resolution_ = 12;
    bool wait_ = true;
    bool requested_ = false;
};
