#pragma once
// esp32emu — Adafruit ENS160 Digital Multi-Gas Sensor mock (AQI, TVOC, eCO2)

#include <Arduino.h>
#include <Wire.h>
#include <cstdint>

class Adafruit_ENS160 {
public:
    Adafruit_ENS160() = default;

    bool begin(TwoWire* wire = nullptr, uint8_t addr = 0x53) {
        (void)wire;
        _address = addr;
        _initialized = true;
        return true;
    }

    // Operating mode: 0=deep sleep, 1=idle, 2=standard, 0xF0=reset
    bool setMode(uint8_t mode) { _mode = mode; return true; }
    uint8_t getMode() const { return _mode; }

    // Air Quality Index (1-5)
    uint8_t getAQI() const { return _aqi; }

    // Total Volatile Organic Compounds (ppb)
    uint16_t getTVOC() const { return _tvoc; }

    // Equivalent CO2 (ppm)
    uint16_t getECO2() const { return _eco2; }

    // Firmware version
    uint16_t getFirmwareVersion() const { return 0x0207; }

    // Set temperature compensation (°C)
    void setTempCompensation(float temp) { _tempComp = temp; }

    // Set humidity compensation (%RH)
    void setRHCompensation(float rh) { _rhComp = rh; }

    // Data ready
    bool dataReady() const { return _dataReady; }

    // Status flags
    uint8_t getStatus() const { return _initialized ? 0x80 : 0x00; }

    // --- Test helpers ---
    void test_setAQI(uint8_t aqi) { _aqi = aqi; }
    void test_setTVOC(uint16_t tvoc) { _tvoc = tvoc; }
    void test_setECO2(uint16_t eco2) { _eco2 = eco2; }
    void test_setDataReady(bool ready) { _dataReady = ready; }

private:
    uint8_t _address = 0x53;
    bool _initialized = false;
    uint8_t _mode = 0;
    uint8_t _aqi = 1;
    uint16_t _tvoc = 0;
    uint16_t _eco2 = 400;
    float _tempComp = 25.0f;
    float _rhComp = 50.0f;
    bool _dataReady = true;
};
