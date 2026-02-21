#pragma once
// esp32emu — CCS811 Air Quality Sensor mock (eCO2 + TVOC)

#include <Arduino.h>
#include <Wire.h>
#include <cstdint>

class CCS811 {
public:
    // Error codes
    enum CCS811_Status : uint8_t {
        CCS811_Stat_SUCCESS = 0,
        CCS811_Stat_ID_ERROR = 1,
        CCS811_Stat_I2C_ERROR = 2,
        CCS811_Stat_INTERNAL_ERROR = 3,
        CCS811_Stat_GENERIC_ERROR = 4
    };

    CCS811(uint8_t addr = 0x5B) : _addr(addr) {}

    bool begin(TwoWire& wire = Wire) {
        (void)wire;
        _initialized = true;
        return true;
    }

    CCS811_Status beginWithStatus(TwoWire& wire = Wire) {
        (void)wire;
        _initialized = true;
        return CCS811_Stat_SUCCESS;
    }

    bool dataAvailable() { return _initialized && _dataReady; }

    void readAlgorithmResults() {
        // In real hardware, reads from sensor registers
    }

    uint16_t getCO2() { return _co2; }
    uint16_t getTVOC() { return _tvoc; }

    // Set environmental data for compensation
    void setEnvironmentalData(float humidity, float temperature) {
        _humidity = humidity;
        _temperature = temperature;
    }

    // Drive mode: 0=idle, 1=1s, 2=10s, 3=60s, 4=250ms
    bool setDriveMode(uint8_t mode) {
        if (mode > 4) return false;
        _driveMode = mode;
        return true;
    }

    // Interrupt control
    void enableInterrupt() { _interruptEnabled = true; }
    void disableInterrupt() { _interruptEnabled = false; }

    // Baseline for calibration
    uint16_t getBaseline() { return _baseline; }
    void setBaseline(uint16_t baseline) { _baseline = baseline; }

    // Check error
    bool checkForStatusError() { return _hasError; }
    const char* getErrorRegister() { return _hasError ? "MOCK_ERROR" : ""; }

    String getHardwareID() { return "0x81"; }

    // --- Test helpers ---
    void test_setCO2(uint16_t co2) { _co2 = co2; }
    void test_setTVOC(uint16_t tvoc) { _tvoc = tvoc; }
    void test_setDataReady(bool ready) { _dataReady = ready; }
    void test_setError(bool err) { _hasError = err; }
    bool test_isInitialized() const { return _initialized; }
    uint8_t test_getDriveMode() const { return _driveMode; }

private:
    uint8_t _addr;
    bool _initialized = false;
    bool _dataReady = true;
    bool _hasError = false;
    bool _interruptEnabled = false;
    uint16_t _co2 = 400;
    uint16_t _tvoc = 0;
    uint16_t _baseline = 0xA1B2;
    uint8_t _driveMode = 1;
    float _humidity = 50.0f;
    float _temperature = 25.0f;
};
