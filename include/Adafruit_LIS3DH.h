#pragma once
// esp32emu — Adafruit LIS3DH 3-axis accelerometer mock

#include <cstdint>
#include <cstdio>
#include <cmath>

// Data rate constants
#define LIS3DH_DATARATE_400_HZ  0x07
#define LIS3DH_DATARATE_200_HZ  0x06
#define LIS3DH_DATARATE_100_HZ  0x05
#define LIS3DH_DATARATE_50_HZ   0x04
#define LIS3DH_DATARATE_25_HZ   0x03
#define LIS3DH_DATARATE_10_HZ   0x02
#define LIS3DH_DATARATE_1_HZ    0x01

// Range constants
#define LIS3DH_RANGE_16_G  0x03
#define LIS3DH_RANGE_8_G   0x02
#define LIS3DH_RANGE_4_G   0x01
#define LIS3DH_RANGE_2_G   0x00

struct sensors_event_t {
    struct { float x, y, z; } acceleration;
};

class Adafruit_LIS3DH {
public:
    Adafruit_LIS3DH() = default;

    bool begin(uint8_t addr = 0x18) {
        addr_ = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] LIS3DH initialized at 0x%02X\n", addr_);
        return true;
    }

    void setRange(uint8_t range) { range_ = range; }
    uint8_t getRange() const { return range_; }

    void setDataRate(uint8_t rate) { data_rate_ = rate; }
    uint8_t getDataRate() const { return data_rate_; }

    void setClick(uint8_t c, uint8_t thresh, uint8_t timelimit = 10,
                  uint8_t timelatency = 20, uint8_t timewindow = 255) {
        click_cfg_ = c; click_thresh_ = thresh;
        (void)timelimit; (void)timelatency; (void)timewindow;
    }
    uint8_t getClick() const { return click_detected_ ? click_cfg_ : 0; }

    bool read() {
        x = raw_x_; y = raw_y_; z = raw_z_;
        return true;
    }

    bool getEvent(sensors_event_t* event) {
        if (!event) return false;
        // LIS3DH: 16-bit raw, sensitivity depends on range
        // ±2G: 16384 LSB/g, ±4G: 8192, ±8G: 4096, ±16G: 2048
        float divider = rangeDivider();
        event->acceleration.x = raw_x_ / divider * 9.806f;
        event->acceleration.y = raw_y_ / divider * 9.806f;
        event->acceleration.z = raw_z_ / divider * 9.806f;
        return true;
    }

    int16_t x = 0, y = 0, z = 0;

    uint8_t getAddress() const { return addr_; }
    bool isInitialized() const { return initialized_; }

    // ── Test helpers ──
    void setRawData(int16_t ax, int16_t ay, int16_t az) {
        raw_x_ = ax; raw_y_ = ay; raw_z_ = az;
    }
    void setClickDetected(bool d) { click_detected_ = d; }

private:
    float rangeDivider() const {
        switch (range_) {
            case LIS3DH_RANGE_16_G: return 2048.0f;
            case LIS3DH_RANGE_8_G:  return 4096.0f;
            case LIS3DH_RANGE_4_G:  return 8192.0f;
            default: return 16384.0f;
        }
    }

    uint8_t addr_ = 0x18;
    bool initialized_ = false;
    uint8_t range_ = LIS3DH_RANGE_2_G;
    uint8_t data_rate_ = LIS3DH_DATARATE_400_HZ;
    uint8_t click_cfg_ = 0;
    uint8_t click_thresh_ = 0;
    bool click_detected_ = false;
    int16_t raw_x_ = 0, raw_y_ = 0, raw_z_ = 16384; // 1G on Z axis
};
