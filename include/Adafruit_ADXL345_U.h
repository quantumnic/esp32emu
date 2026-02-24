#pragma once
#include <cstdint>
#include <cstdio>
#include <cmath>

// ADXL345 3-axis accelerometer mock
typedef enum {
    ADXL345_RANGE_16_G = 0b11,
    ADXL345_RANGE_8_G  = 0b10,
    ADXL345_RANGE_4_G  = 0b01,
    ADXL345_RANGE_2_G  = 0b00
} range_t;

typedef enum {
    ADXL345_DATARATE_3200_HZ = 0b1111,
    ADXL345_DATARATE_1600_HZ = 0b1110,
    ADXL345_DATARATE_800_HZ  = 0b1101,
    ADXL345_DATARATE_400_HZ  = 0b1100,
    ADXL345_DATARATE_200_HZ  = 0b1011,
    ADXL345_DATARATE_100_HZ  = 0b1010,
    ADXL345_DATARATE_50_HZ   = 0b1001,
    ADXL345_DATARATE_25_HZ   = 0b1000,
    ADXL345_DATARATE_12_5_HZ = 0b0111,
    ADXL345_DATARATE_6_25HZ  = 0b0110,
    ADXL345_DATARATE_3_13_HZ = 0b0101,
    ADXL345_DATARATE_1_56_HZ = 0b0100,
    ADXL345_DATARATE_0_78_HZ = 0b0011,
    ADXL345_DATARATE_0_39_HZ = 0b0010,
    ADXL345_DATARATE_0_20_HZ = 0b0001,
    ADXL345_DATARATE_0_10_HZ = 0b0000
} dataRate_t;

struct sensors_event_t_adxl {
    struct { float x, y, z; } acceleration;
};

class Adafruit_ADXL345_Unified {
public:
    Adafruit_ADXL345_Unified(int32_t sensorID = -1) { (void)sensorID; }

    bool begin(uint8_t addr = 0x53) { (void)addr; started_ = true; return true; }

    void setRange(range_t range) { range_ = range; }
    range_t getRange() { return range_; }

    void setDataRate(dataRate_t rate) { data_rate_ = rate; }
    dataRate_t getDataRate() { return data_rate_; }

    bool getEvent(sensors_event_t_adxl* event) {
        if (!event) return false;
        event->acceleration.x = accel_x_;
        event->acceleration.y = accel_y_;
        event->acceleration.z = accel_z_;
        return true;
    }

    // Tap detection
    void setTapThreshold(uint8_t t) { tap_threshold_ = t; }
    uint8_t getTapThreshold() { return tap_threshold_; }
    void setTapDuration(uint8_t d) { tap_duration_ = d; }
    uint8_t getTapDuration() { return tap_duration_; }
    void setDoubleTapLatency(uint8_t l) { double_tap_latency_ = l; }
    void setDoubleTapWindow(uint8_t w) { double_tap_window_ = w; }

    // Activity/inactivity
    void setActivityThreshold(uint8_t t) { activity_threshold_ = t; }
    void setInactivityThreshold(uint8_t t) { inactivity_threshold_ = t; }
    void setInactivityTime(uint8_t t) { inactivity_time_ = t; }

    // Free-fall
    void setFreeFallThreshold(uint8_t t) { freefall_threshold_ = t; }
    void setFreeFallDuration(uint8_t d) { freefall_duration_ = d; }

    // Interrupts
    uint8_t getInterruptSource() { return int_source_; }

    // Test helpers
    void test_setAccel(float x, float y, float z) { accel_x_ = x; accel_y_ = y; accel_z_ = z; }
    void test_setInterruptSource(uint8_t src) { int_source_ = src; }
    bool test_isStarted() const { return started_; }
    float test_getPitch() const { return atan2(accel_x_, sqrt(accel_y_*accel_y_ + accel_z_*accel_z_)) * 180.0f / M_PI; }
    float test_getRoll() const { return atan2(accel_y_, sqrt(accel_x_*accel_x_ + accel_z_*accel_z_)) * 180.0f / M_PI; }

private:
    bool started_ = false;
    range_t range_ = ADXL345_RANGE_2_G;
    dataRate_t data_rate_ = ADXL345_DATARATE_100_HZ;
    float accel_x_ = 0.0f, accel_y_ = 0.0f, accel_z_ = 9.81f;
    uint8_t tap_threshold_ = 0, tap_duration_ = 0;
    uint8_t double_tap_latency_ = 0, double_tap_window_ = 0;
    uint8_t activity_threshold_ = 0, inactivity_threshold_ = 0, inactivity_time_ = 0;
    uint8_t freefall_threshold_ = 0, freefall_duration_ = 0;
    uint8_t int_source_ = 0;
};
