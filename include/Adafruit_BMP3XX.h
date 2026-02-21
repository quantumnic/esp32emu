#pragma once
// esp32emu — Adafruit BMP3XX (BMP388/BMP390) Pressure Sensor mock

#include <cstdint>
#include <cstdio>
#include <cmath>

#define BMP3XX_DEFAULT_ADDRESS 0x77

// Oversampling
#define BMP3_NO_OVERSAMPLING   0
#define BMP3_OVERSAMPLING_2X   1
#define BMP3_OVERSAMPLING_4X   2
#define BMP3_OVERSAMPLING_8X   3
#define BMP3_OVERSAMPLING_16X  4
#define BMP3_OVERSAMPLING_32X  5

// IIR filter coefficients
#define BMP3_IIR_FILTER_DISABLE  0
#define BMP3_IIR_FILTER_COEFF_1  1
#define BMP3_IIR_FILTER_COEFF_3  2
#define BMP3_IIR_FILTER_COEFF_7  3
#define BMP3_IIR_FILTER_COEFF_15 4

// Output data rates
#define BMP3_ODR_200_HZ  0x00
#define BMP3_ODR_100_HZ  0x01
#define BMP3_ODR_50_HZ   0x02
#define BMP3_ODR_25_HZ   0x03

class Adafruit_BMP3XX {
public:
    Adafruit_BMP3XX() = default;

    bool begin_I2C(uint8_t addr = BMP3XX_DEFAULT_ADDRESS) {
        addr_ = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] BMP3XX initialized at 0x%02X\n", addr_);
        return true;
    }

    bool begin_SPI(int8_t cs) { (void)cs; initialized_ = true; return true; }

    bool performReading() {
        // In mock, readings are always "ready"
        return true;
    }

    // Set oversampling
    bool setTemperatureOversampling(uint8_t os) { temp_os_ = os; return true; }
    bool setPressureOversampling(uint8_t os) { pres_os_ = os; return true; }
    bool setIIRFilterCoeff(uint8_t coeff) { iir_ = coeff; return true; }
    bool setOutputDataRate(uint8_t odr) { odr_ = odr; return true; }

    // Readings
    float temperature = 22.0f;   // °C
    float pressure = 101325.0f;  // Pa

    // Altitude from pressure using barometric formula
    float readAltitude(float seaLevelPa = 101325.0f) const {
        return 44330.0f * (1.0f - powf(pressure / seaLevelPa, 0.1903f));
    }

    uint8_t getAddress() const { return addr_; }
    bool isInitialized() const { return initialized_; }

    // ── Test helpers ──
    void setTemperature(float t) { temperature = t; }
    void setPressure(float p) { pressure = p; }
    void setReadings(float temp, float pres) {
        temperature = temp; pressure = pres;
    }

private:
    uint8_t addr_ = BMP3XX_DEFAULT_ADDRESS;
    bool initialized_ = false;
    uint8_t temp_os_ = BMP3_NO_OVERSAMPLING;
    uint8_t pres_os_ = BMP3_NO_OVERSAMPLING;
    uint8_t iir_ = BMP3_IIR_FILTER_DISABLE;
    uint8_t odr_ = BMP3_ODR_50_HZ;
};
