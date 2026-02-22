#pragma once
// esp32emu — Adafruit SHT4x (SHT40/SHT41/SHT45) temperature & humidity sensor mock

#include <cstdint>
#include <cstdio>
#include <cmath>

typedef enum {
    SHT4X_HIGH_PRECISION = 0xFD,
    SHT4X_MED_PRECISION  = 0xF6,
    SHT4X_LOW_PRECISION  = 0xE0,
} sht4x_precision_t;

typedef enum {
    SHT4X_NO_HEATER      = 0,
    SHT4X_HIGH_HEATER_1S = 0x39,
    SHT4X_HIGH_HEATER_100MS = 0x32,
    SHT4X_MED_HEATER_1S  = 0x2F,
    SHT4X_MED_HEATER_100MS = 0x24,
    SHT4X_LOW_HEATER_1S  = 0x1E,
    SHT4X_LOW_HEATER_100MS = 0x15,
} sht4x_heater_t;

struct sensors_event_t {
    float temperature;
    float relative_humidity;
};

class Adafruit_SHT4x {
public:
    Adafruit_SHT4x() = default;

    bool begin(uint8_t addr = 0x44) {
        (void)addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] SHT4x: initialized at 0x%02X\n", addr);
        return true;
    }

    void setPrecision(sht4x_precision_t prec) { precision_ = prec; }
    sht4x_precision_t getPrecision() const { return precision_; }

    void setHeater(sht4x_heater_t heater) { heater_ = heater; }
    sht4x_heater_t getHeater() const { return heater_; }

    bool getEvent(sensors_event_t* humidity, sensors_event_t* temp) {
        if (!initialized_) return false;
        if (temp) temp->temperature = temperature_;
        if (humidity) humidity->relative_humidity = humidity_;
        return true;
    }

    void reset() {
        fprintf(stderr, "[esp32emu] SHT4x: soft reset\n");
    }

    uint32_t readSerial() { return serial_; }

    // Test helpers
    void esp32emu_set_temperature(float t) { temperature_ = t; }
    void esp32emu_set_humidity(float h) { humidity_ = h; }
    void esp32emu_set_serial(uint32_t s) { serial_ = s; }

private:
    bool initialized_ = false;
    sht4x_precision_t precision_ = SHT4X_HIGH_PRECISION;
    sht4x_heater_t heater_ = SHT4X_NO_HEATER;
    float temperature_ = 22.5f;
    float humidity_ = 45.0f;
    uint32_t serial_ = 0x12345678;
};
