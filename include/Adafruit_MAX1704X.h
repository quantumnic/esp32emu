// esp32emu — Adafruit_MAX1704X mock: battery fuel gauge (I2C)
#pragma once
#include <cstdint>
#include <cstdio>

class Adafruit_MAX17048 {
public:
    Adafruit_MAX17048() : _begun(false), _voltage(3.7f), _percent(75.0f), _rate(0.5f) {}

    bool begin(void* wire = nullptr) {
        (void)wire;
        _begun = true;
        printf("[esp32emu] MAX17048: initialized\n");
        return true;
    }

    float cellVoltage() const { return _voltage; }
    float cellPercent() const { return _percent; }
    float chargeRate() const { return _rate; }

    bool isActiveAlert() const { return _percent < 10.0f; }

    void quickStart() {
        printf("[esp32emu] MAX17048: quick start\n");
    }

    void sleep() {
        printf("[esp32emu] MAX17048: sleep\n");
    }

    void wake() {
        printf("[esp32emu] MAX17048: wake\n");
    }

    void setActivityThreshold(float threshold) { _activityThreshold = threshold; }
    void setAlertVoltages(float minV, float maxV) { _alertMinV = minV; _alertMaxV = maxV; (void)_alertMinV; (void)_alertMaxV; }

    // --- Test helpers ---
    void test_setVoltage(float v) { _voltage = v; }
    void test_setPercent(float p) { _percent = p; }
    void test_setRate(float r) { _rate = r; }
    bool test_isBegun() const { return _begun; }
    float test_getVoltage() const { return _voltage; }
    float test_getPercent() const { return _percent; }

private:
    bool _begun;
    float _voltage;
    float _percent;
    float _rate;
    float _activityThreshold = 0.0f;
    float _alertMinV = 0.0f;
    float _alertMaxV = 5.0f;
};
