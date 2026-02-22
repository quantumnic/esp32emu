#pragma once
// esp32emu — MQ135 air quality sensor mock
// Gas sensor for CO2, NH3, NOx, alcohol, benzene, smoke detection.

#include "Arduino.h"
#include <cstdint>
#include <cstdio>
#include <cmath>

class MQ135 {
public:
    // Calibration constants (from datasheet curves)
    static constexpr float PARA = 116.6020682f;
    static constexpr float PARB = 2.769034857f;
    static constexpr float CORA = 0.00035f;
    static constexpr float CORB = 0.02718f;
    static constexpr float CORC = 1.39538f;
    static constexpr float CORD = 0.0018f;
    static constexpr float CORE = -0.003333333f;
    static constexpr float CORF = -0.001923077f;
    static constexpr float CORG = 1.130128205f;
    static constexpr float ATMOCO2 = 397.13f;  // ppm
    static constexpr float RLOAD = 10.0f;       // kOhm
    static constexpr float RZERO = 76.63f;      // kOhm (calibrated in clean air)

    explicit MQ135(uint8_t pin) : _pin(pin) {
        fprintf(stderr, "[esp32emu] MQ135: init on pin %d\n", pin);
    }

    // Read raw resistance ratio
    float getCorrectionFactor(float temperature, float humidity) const {
        if (temperature < 20) {
            return CORA * temperature * temperature
                 - CORB * temperature + CORC
                 - (humidity - 33.0f) * CORD;
        }
        return CORE * temperature + CORF * humidity + CORG;
    }

    float getResistance() const {
        int val = _inject_raw >= 0 ? _inject_raw : analogRead(_pin);
        return ((1023.0f / (float)val) - 1.0f) * RLOAD;
    }

    float getCorrectedResistance(float temperature, float humidity) const {
        return getResistance() / getCorrectionFactor(temperature, humidity);
    }

    float getPPM() const {
        return PARA * pow(getResistance() / RZERO, -PARB);
    }

    float getCorrectedPPM(float temperature, float humidity) const {
        return PARA * pow(getCorrectedResistance(temperature, humidity) / RZERO, -PARB);
    }

    float getRZero() const {
        return getResistance() * pow(ATMOCO2 / PARA, 1.0f / -PARB);
    }

    float getCorrectedRZero(float temperature, float humidity) const {
        return getCorrectedResistance(temperature, humidity)
             * pow(ATMOCO2 / PARA, 1.0f / -PARB);
    }

    uint8_t getPin() const { return _pin; }

    // Test helpers
    void _injectRaw(int value) { _inject_raw = value; }
    void _resetInjection() { _inject_raw = -1; }

private:
    uint8_t _pin;
    mutable int _inject_raw = -1;
};
