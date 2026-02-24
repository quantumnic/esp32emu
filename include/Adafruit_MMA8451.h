#pragma once
// esp32emu — Adafruit MMA8451 Triple-Axis Accelerometer mock
// NXP MMA8451Q 14-bit, ±2/4/8g I2C accelerometer.

#include <cstdint>
#include <cstdio>
#include <cmath>

// Range constants
#define MMA8451_RANGE_8_G  0
#define MMA8451_RANGE_4_G  1
#define MMA8451_RANGE_2_G  2

// Data rate constants
#define MMA8451_DATARATE_800_HZ   0
#define MMA8451_DATARATE_400_HZ   1
#define MMA8451_DATARATE_200_HZ   2
#define MMA8451_DATARATE_100_HZ   3
#define MMA8451_DATARATE_50_HZ    4
#define MMA8451_DATARATE_12_5_HZ  5
#define MMA8451_DATARATE_6_25HZ   6
#define MMA8451_DATARATE_1_56_HZ  7

typedef uint8_t mma8451_range_t;
typedef uint8_t mma8451_dataRate_t;

class Adafruit_MMA8451 {
public:
    Adafruit_MMA8451(uint8_t addr = 0x1D) : addr_(addr) {}

    bool begin(uint8_t /*i2caddr*/ = 0x1D) {
        initialized_ = true;
        fprintf(stderr, "[esp32emu] MMA8451 initialized at 0x%02X\n", addr_);
        return true;
    }

    void read() {
        // In real hardware, this triggers a read from registers.
        // Mock: values are already set via test helpers.
    }

    // Raw 14-bit values
    int16_t x = 0, y = 0, z = 0;

    // Acceleration in m/s² (filled after getEvent or manually)
    float x_g = 0.0f, y_g = 0.0f, z_g = 1.0f;

    void setRange(mma8451_range_t range) { range_ = range; }
    mma8451_range_t getRange() const { return range_; }

    void setDataRate(mma8451_dataRate_t rate) { datarate_ = rate; }
    mma8451_dataRate_t getDataRate() const { return datarate_; }

    uint8_t getOrientation() const {
        // Simplified: 0=portrait up, 1=portrait down, 2=landscape right, 3=landscape left
        if (std::fabs(z_g) > std::fabs(x_g) && std::fabs(z_g) > std::fabs(y_g)) return 0;
        if (std::fabs(x_g) > std::fabs(y_g)) return x_g > 0 ? 2 : 3;
        return y_g > 0 ? 0 : 1;
    }

    bool isInitialized() const { return initialized_; }
    uint8_t getAddress() const { return addr_; }

    // ── Test helpers ──
    void setAcceleration(float gx, float gy, float gz) {
        x_g = gx; y_g = gy; z_g = gz;
        // Approximate raw 14-bit values (±2g default: 4096 counts/g)
        float scale = 4096.0f;
        if (range_ == MMA8451_RANGE_4_G) scale = 2048.0f;
        if (range_ == MMA8451_RANGE_8_G) scale = 1024.0f;
        x = static_cast<int16_t>(gx * scale);
        y = static_cast<int16_t>(gy * scale);
        z = static_cast<int16_t>(gz * scale);
    }

private:
    uint8_t addr_;
    bool initialized_ = false;
    mma8451_range_t range_ = MMA8451_RANGE_2_G;
    mma8451_dataRate_t datarate_ = MMA8451_DATARATE_800_HZ;
};
