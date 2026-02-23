#pragma once
// esp32emu — Adafruit AHTX0 (AHT10/AHT20) temperature & humidity sensor mock
// Uses Adafruit Unified Sensor event-based API.

#include <cstdint>
#include <cstdio>

class Adafruit_AHTX0 {
public:
    struct sensors_event_t {
        float temperature;
        float relative_humidity;
    };

    Adafruit_AHTX0() = default;

    bool begin(void* /*wire*/ = nullptr, int32_t /*id*/ = 0) {
        initialized_ = true;
        fprintf(stderr, "[esp32emu] AHTX0: initialized\n");
        return true;
    }

    bool getEvent(sensors_event_t* humidity, sensors_event_t* temp) {
        if (!initialized_) return false;
        if (temp) temp->temperature = temperature_;
        if (humidity) humidity->relative_humidity = humidity_;
        return true;
    }

    uint8_t getStatus() {
        return initialized_ ? 0x18 : 0xFF;
    }

    // Test helpers
    void _setTemperature(float t)  { temperature_ = t; }
    void _setHumidity(float h)     { humidity_ = h; }
    bool _isInitialized() const    { return initialized_; }

private:
    bool initialized_ = false;
    float temperature_ = 23.0f;
    float humidity_ = 50.0f;
};
