// Adafruit_BME68x.h — BME68x (BSEC2-compatible) sensor mock for esp32emu
#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>

// Gas heater profiles
#define BME68X_OS_NONE    0
#define BME68X_OS_1X      1
#define BME68X_OS_2X      2
#define BME68X_OS_4X      3
#define BME68X_OS_8X      4
#define BME68X_OS_16X     5

#define BME68X_FILTER_OFF    0
#define BME68X_FILTER_SIZE_1 1
#define BME68X_FILTER_SIZE_3 2
#define BME68X_FILTER_SIZE_7 3

#define BME68X_ODR_NONE     0
#define BME68X_ODR_0_59_MS  1
#define BME68X_ODR_10_MS    2
#define BME68X_ODR_20_MS    3

// Operating modes
#define BME68X_SLEEP_MODE   0
#define BME68X_FORCED_MODE  1
#define BME68X_PARALLEL_MODE 2

// I2C addresses
#define BME68X_DEFAULT_ADDRESS   0x77
#define BME68X_SECONDARY_ADDRESS 0x76

struct bme68x_data {
    float temperature;
    float pressure;
    float humidity;
    float gas_resistance;
    uint8_t gas_index;
    uint8_t status;
};

class Adafruit_BME68x {
public:
    Adafruit_BME68x() : _addr(BME68X_DEFAULT_ADDRESS) {}

    bool begin(uint8_t addr = BME68X_DEFAULT_ADDRESS, void* wire = nullptr, int32_t sensorID = 0x61) {
        (void)wire; (void)sensorID;
        _addr = addr;
        _initialized = true;
        printf("[esp32emu] BME68x: initialized at 0x%02X\n", addr);
        return true;
    }

    bool setTemperatureOversampling(uint8_t os) { _tempOS = os; return true; }
    bool setPressureOversampling(uint8_t os) { _pressOS = os; return true; }
    bool setHumidityOversampling(uint8_t os) { _humOS = os; return true; }
    bool setIIRFilterSize(uint8_t fs) { _filter = fs; return true; }
    bool setODR(uint8_t odr) { _odr = odr; return true; }

    bool setGasHeater(uint16_t heaterTemp, uint16_t heaterTime) {
        _heaterTemp = heaterTemp;
        _heaterTime = heaterTime;
        _gasEnabled = true;
        printf("[esp32emu] BME68x: gas heater %d°C for %dms\n", heaterTemp, heaterTime);
        return true;
    }

    // Perform a forced-mode reading
    bool performReading() {
        if (!_initialized) return false;
        _lastData.temperature = _testTemp;
        _lastData.pressure = _testPressure;
        _lastData.humidity = _testHumidity;
        _lastData.gas_resistance = _testGasResistance;
        _lastData.gas_index = 0;
        _lastData.status = 0xB0; // valid measurement
        _readingValid = true;
        return true;
    }

    // End reading (for non-blocking mode)
    int8_t endReading() {
        return performReading() ? 1 : 0;
    }

    // Begin async reading
    uint32_t beginReading() {
        if (!_initialized) return 0;
        return 1; // return non-zero to indicate started
    }

    // Property getters (after performReading)
    float readTemperature() { return _readingValid ? _lastData.temperature : 0; }
    float readPressure() { return _readingValid ? _lastData.pressure : 0; }
    float readHumidity() { return _readingValid ? _lastData.humidity : 0; }
    float readGas() { return _readingValid ? _lastData.gas_resistance : 0; }
    float readAltitude(float seaLevel = 1013.25f) {
        if (!_readingValid) return 0;
        return 44330.0f * (1.0f - powf(_lastData.pressure / 100.0f / seaLevel, 0.1903f));
    }

    // Direct access to sensor data struct
    bme68x_data getData() const { return _lastData; }

    // Remaining reading time
    int32_t remainingReadingMillis() { return 0; }

    // Config getters
    uint8_t getTemperatureOversampling() const { return _tempOS; }
    uint8_t getPressureOversampling() const { return _pressOS; }
    uint8_t getHumidityOversampling() const { return _humOS; }
    uint8_t getIIRFilterSize() const { return _filter; }
    uint16_t getHeaterTemp() const { return _heaterTemp; }
    uint16_t getHeaterTime() const { return _heaterTime; }
    bool isGasEnabled() const { return _gasEnabled; }

    // Test helpers
    void setTestTemperature(float t) { _testTemp = t; }
    void setTestPressure(float p) { _testPressure = p; }
    void setTestHumidity(float h) { _testHumidity = h; }
    void setTestGasResistance(float g) { _testGasResistance = g; }

private:
    uint8_t _addr;
    bool _initialized = false;
    bool _readingValid = false;
    bool _gasEnabled = false;
    uint8_t _tempOS = BME68X_OS_2X;
    uint8_t _pressOS = BME68X_OS_4X;
    uint8_t _humOS = BME68X_OS_1X;
    uint8_t _filter = BME68X_FILTER_OFF;
    uint8_t _odr = BME68X_ODR_NONE;
    uint16_t _heaterTemp = 320;
    uint16_t _heaterTime = 150;
    float _testTemp = 22.5f;
    float _testPressure = 101325.0f;
    float _testHumidity = 45.0f;
    float _testGasResistance = 50000.0f;
    bme68x_data _lastData = {};
};
