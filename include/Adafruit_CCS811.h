#pragma once
// esp32emu: Adafruit CCS811 Air Quality Sensor mock (eCO2 + TVOC)
#include <cstdint>
#include <cstdio>

class Adafruit_CCS811 {
public:
    enum CCS811_driveMode {
        CCS811_DRIVE_MODE_IDLE = 0,
        CCS811_DRIVE_MODE_1SEC = 1,
        CCS811_DRIVE_MODE_10SEC = 2,
        CCS811_DRIVE_MODE_60SEC = 3,
        CCS811_DRIVE_MODE_250MS = 4
    };

    Adafruit_CCS811() = default;

    bool begin(uint8_t addr = 0x5A, void* i2c = nullptr) {
        (void)i2c;
        _addr = addr;
        _begun = true;
        printf("[esp32emu] Adafruit_CCS811: begin (addr=0x%02X)\n", _addr);
        return true;
    }

    bool available() { return _begun && _dataReady; }

    uint8_t readData() {
        // In real hw reads eCO2 + TVOC registers; return 0 = success
        return _begun ? 0 : 1;
    }

    uint16_t geteCO2() { return _eco2; }
    uint16_t getTVOC() { return _tvoc; }

    // Temperature from onboard NTC
    float calculateTemperature() { return _temperature; }

    // Environmental compensation
    void setEnvironmentalData(float humidity, float temperature) {
        _humidity = humidity;
        _temperature = temperature;
    }

    // Drive mode
    void setDriveMode(uint8_t mode) { _driveMode = mode; }

    // Interrupt
    void enableInterrupt() { _intEnabled = true; }
    void disableInterrupt() { _intEnabled = false; }

    // Threshold interrupt
    void setThresholds(uint16_t low, uint16_t high, uint8_t hysteresis = 50) {
        _threshLow = low; _threshHigh = high; _hysteresis = hysteresis;
    }

    // Baseline
    uint16_t getBaseline() { return _baseline; }
    void setBaseline(uint16_t baseline) { _baseline = baseline; }

    // Software reset
    void SWReset() { _eco2 = 400; _tvoc = 0; }

    // Check error
    bool checkError() { return _error; }

    // Test helpers
    void setECO2(uint16_t val) { _eco2 = val; }
    void setTVOC(uint16_t val) { _tvoc = val; }
    void setDataReady(bool ready) { _dataReady = ready; }
    void setTemperature(float t) { _temperature = t; }
    void setError(bool e) { _error = e; }
    bool isBegun() const { return _begun; }
    uint8_t getDriveMode() const { return _driveMode; }

private:
    uint8_t _addr = 0x5A;
    bool _begun = false;
    bool _dataReady = true;
    uint16_t _eco2 = 400;
    uint16_t _tvoc = 0;
    float _temperature = 25.0f;
    float _humidity = 50.0f;
    uint8_t _driveMode = CCS811_DRIVE_MODE_1SEC;
    bool _intEnabled = false;
    uint16_t _threshLow = 400;
    uint16_t _threshHigh = 2000;
    uint8_t _hysteresis = 50;
    uint16_t _baseline = 0;
    bool _error = false;
};
