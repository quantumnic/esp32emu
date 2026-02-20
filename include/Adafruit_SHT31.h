#pragma once
// esp32emu — Adafruit SHT31 temperature/humidity sensor mock

#include <cstdint>
#include <cstdio>
#include <cmath>

class Adafruit_SHT31 {
public:
    Adafruit_SHT31() = default;

    bool begin(uint8_t addr = 0x44, void* /*wire*/ = nullptr) {
        addr_ = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] SHT31: initialized at 0x%02X\n", addr);
        return true;
    }

    float readTemperature() {
        if (!initialized_) return NAN;
        return temperature_;
    }

    float readHumidity() {
        if (!initialized_) return NAN;
        return humidity_;
    }

    bool readBoth(float* temp, float* hum) {
        if (!initialized_) return false;
        if (temp) *temp = temperature_;
        if (hum)  *hum = humidity_;
        return true;
    }

    void heater(bool on) {
        heater_on_ = on;
        fprintf(stderr, "[esp32emu] SHT31: heater %s\n", on ? "ON" : "OFF");
    }

    bool isHeaterEnabled() const { return heater_on_; }

    uint16_t readStatus() { return status_; }

    void reset() {
        fprintf(stderr, "[esp32emu] SHT31: soft reset\n");
    }

    // Test helpers
    void _setTemperature(float t) { temperature_ = t; }
    void _setHumidity(float h)    { humidity_ = h; }
    void _setStatus(uint16_t s)   { status_ = s; }
    bool _isInitialized() const   { return initialized_; }
    uint8_t _getAddr() const      { return addr_; }

private:
    uint8_t addr_ = 0x44;
    bool initialized_ = false;
    bool heater_on_ = false;
    float temperature_ = 23.0f;
    float humidity_ = 50.0f;
    uint16_t status_ = 0;
};
