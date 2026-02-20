#pragma once
// esp32emu — Adafruit BME680 gas sensor mock
// Temperature, humidity, pressure, and gas resistance (air quality).

#include <cstdint>
#include <cstdio>
#include <cmath>

#define BME680_OS_NONE   0
#define BME680_OS_1X     1
#define BME680_OS_2X     2
#define BME680_OS_4X     3
#define BME680_OS_8X     4
#define BME680_OS_16X    5

#define BME680_FILTER_SIZE_0   0
#define BME680_FILTER_SIZE_1   1
#define BME680_FILTER_SIZE_3   2
#define BME680_FILTER_SIZE_7   3
#define BME680_FILTER_SIZE_15  4
#define BME680_FILTER_SIZE_31  5
#define BME680_FILTER_SIZE_63  6
#define BME680_FILTER_SIZE_127 7

class Adafruit_BME680 {
public:
    Adafruit_BME680() = default;

    bool begin(uint8_t addr = 0x77, void* /*wire*/ = nullptr) {
        addr_ = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] BME680: initialized at 0x%02X\n", addr);
        return true;
    }

    // Oversampling
    void setTemperatureOversampling(uint8_t os) { temp_os_ = os; }
    void setHumidityOversampling(uint8_t os)    { hum_os_ = os; }
    void setPressureOversampling(uint8_t os)    { pres_os_ = os; }

    // IIR filter
    void setIIRFilterSize(uint8_t size) { filter_ = size; }

    // Gas heater
    void setGasHeater(uint16_t temp, uint16_t duration_ms) {
        heater_temp_ = temp;
        heater_duration_ = duration_ms;
    }

    bool performReading() {
        if (!initialized_) return false;
        reading_done_ = true;
        return true;
    }

    // After performReading(), these are available:
    float temperature = 22.5f;   // °C
    float humidity    = 45.0f;   // %
    float pressure    = 101325.0f; // Pa
    float gas_resistance = 50000.0f; // Ohms

    // Altitude from pressure
    float readAltitude(float seaLevel = 1013.25f) {
        float atm = pressure / 100.0f;
        return 44330.0f * (1.0f - powf(atm / seaLevel, 0.1903f));
    }

    // Test helpers
    void _setTemperature(float t)      { temperature = t; }
    void _setHumidity(float h)         { humidity = h; }
    void _setPressure(float p)         { pressure = p; }
    void _setGasResistance(float g)    { gas_resistance = g; }

    uint8_t _getAddr() const           { return addr_; }
    bool _isInitialized() const        { return initialized_; }
    uint8_t _getTempOversampling() const { return temp_os_; }
    uint8_t _getHumOversampling() const  { return hum_os_; }
    uint8_t _getPresOversampling() const { return pres_os_; }
    uint16_t _getHeaterTemp() const      { return heater_temp_; }
    uint16_t _getHeaterDuration() const  { return heater_duration_; }

private:
    uint8_t addr_ = 0x77;
    bool initialized_ = false;
    bool reading_done_ = false;
    uint8_t temp_os_ = BME680_OS_1X;
    uint8_t hum_os_ = BME680_OS_1X;
    uint8_t pres_os_ = BME680_OS_1X;
    uint8_t filter_ = BME680_FILTER_SIZE_0;
    uint16_t heater_temp_ = 320;
    uint16_t heater_duration_ = 150;
};
