#pragma once
// esp32emu — Adafruit TMP117 high-accuracy I2C temperature sensor mock

#include <cstdint>
#include <cstdio>
#include <cmath>

#define TMP117_RESOLUTION 0.0078125f

typedef enum {
    TMP117_RATE_ONE_SHOT = 0,
    TMP117_RATE_1_HZ     = 1,
    TMP117_RATE_4_HZ     = 2,
    TMP117_RATE_16_HZ    = 3,
    TMP117_RATE_64_HZ    = 4,
} tmp117_rate_t;

typedef enum {
    TMP117_AVERAGE_1X   = 0,
    TMP117_AVERAGE_8X   = 1,
    TMP117_AVERAGE_32X  = 2,
    TMP117_AVERAGE_64X  = 3,
} tmp117_average_t;

typedef enum {
    TMP117_ALERT_NONE   = 0,
    TMP117_ALERT_HIGH   = 1,
    TMP117_ALERT_LOW    = 2,
} tmp117_alert_t;

class Adafruit_TMP117 {
public:
    Adafruit_TMP117() = default;

    bool begin(uint8_t addr = 0x48) {
        addr_ = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] TMP117: initialized at 0x%02X\n", addr);
        return true;
    }

    float readTempC() {
        data_ready_ = false;
        return temperature_;
    }

    bool dataReady() const { return data_ready_; }

    void setAveraging(tmp117_average_t avg) { averaging_ = avg; }
    tmp117_average_t getAveraging() const { return averaging_; }

    void setRate(tmp117_rate_t rate) { rate_ = rate; }
    tmp117_rate_t getRate() const { return rate_; }

    void setHighThreshold(float t) { high_thresh_ = t; }
    float getHighThreshold() const { return high_thresh_; }

    void setLowThreshold(float t) { low_thresh_ = t; }
    float getLowThreshold() const { return low_thresh_; }

    tmp117_alert_t getAlert() const {
        if (temperature_ > high_thresh_) return TMP117_ALERT_HIGH;
        if (temperature_ < low_thresh_) return TMP117_ALERT_LOW;
        return TMP117_ALERT_NONE;
    }

    void setOffset(float offset) { offset_ = offset; }
    float getOffset() const { return offset_; }

    void softReset() {
        fprintf(stderr, "[esp32emu] TMP117: soft reset\n");
        rate_ = TMP117_RATE_4_HZ;
        averaging_ = TMP117_AVERAGE_8X;
    }

    uint16_t getDeviceID() const { return 0x0117; }

    // Test helpers
    void esp32emu_set_temperature(float t) { temperature_ = t; data_ready_ = true; }
    void esp32emu_set_data_ready(bool r) { data_ready_ = r; }
    float esp32emu_get_temperature() const { return temperature_; }
    bool esp32emu_is_initialized() const { return initialized_; }

private:
    bool initialized_ = false;
    bool data_ready_ = true;
    uint8_t addr_ = 0x48;
    float temperature_ = 23.0f;
    float offset_ = 0.0f;
    float high_thresh_ = 80.0f;
    float low_thresh_ = -40.0f;
    tmp117_rate_t rate_ = TMP117_RATE_4_HZ;
    tmp117_average_t averaging_ = TMP117_AVERAGE_8X;
};
