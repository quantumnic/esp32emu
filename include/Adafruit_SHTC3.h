// esp32emu — Adafruit_SHTC3 mock: humidity & temperature sensor (I2C)
#pragma once
#include <cstdint>
#include <cstdio>

struct sensors_event_t_shtc3 {
    float temperature;
    float relative_humidity;
};

class Adafruit_SHTC3 {
public:
    Adafruit_SHTC3() : _begun(false), _temperature(22.5f), _humidity(55.0f), _sleeping(false) {}

    bool begin(void* wire = nullptr) {
        (void)wire;
        _begun = true;
        printf("[esp32emu] SHTC3: initialized\n");
        return true;
    }

    bool getEvent(sensors_event_t_shtc3* humidity, sensors_event_t_shtc3* temp) {
        if (!_begun) return false;
        if (humidity) humidity->relative_humidity = _humidity;
        if (temp) temp->temperature = _temperature;
        return true;
    }

    bool sleep() {
        _sleeping = true;
        printf("[esp32emu] SHTC3: sleep\n");
        return true;
    }

    bool wakeup() {
        _sleeping = false;
        printf("[esp32emu] SHTC3: wakeup\n");
        return true;
    }

    bool reset() {
        printf("[esp32emu] SHTC3: reset\n");
        return true;
    }

    bool lowPowerMode(bool enable) {
        printf("[esp32emu] SHTC3: low power mode %s\n", enable ? "on" : "off");
        return true;
    }

    // --- Test helpers ---
    void test_setTemperature(float t) { _temperature = t; }
    void test_setHumidity(float h) { _humidity = h; }
    float test_getTemperature() const { return _temperature; }
    float test_getHumidity() const { return _humidity; }
    bool test_isBegun() const { return _begun; }
    bool test_isSleeping() const { return _sleeping; }

private:
    bool _begun;
    float _temperature;
    float _humidity;
    bool _sleeping;
};
