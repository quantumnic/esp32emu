#pragma once
#include <cstdint>
#include <cstdio>

// MH-Z19B CO2 sensor mock
class MHZ19 {
public:
    MHZ19() = default;

    void begin(HardwareSerial& /*serial*/) { started_ = true; }

    int getCO2(bool isunLimited = false) {
        (void)isunLimited;
        return co2_;
    }

    float getTemperature(bool isFloat = true) {
        (void)isFloat;
        return temperature_;
    }

    int getMinCO2() { return min_co2_; }
    int getMaxCO2() { return max_co2_; }
    int getAccuracy() { return accuracy_; }

    void autoCalibration(bool isON = true) { auto_cal_ = isON; }
    bool getABC() { return auto_cal_; }

    void calibrateZero() { zero_calibrated_ = true; }
    void calibrateSpan(int span) { span_ = span; }

    void setRange(int range) { range_ = range; }
    int getRange() { return range_; }

    void recoveryReset() { recovery_reset_ = true; }

    byte getLastResponse(byte index) {
        if (index < 9) return last_response_[index];
        return 0;
    }

    // Test helpers
    void test_setCO2(int ppm) { co2_ = ppm; }
    void test_setTemperature(float t) { temperature_ = t; }
    void test_setMinCO2(int ppm) { min_co2_ = ppm; }
    void test_setMaxCO2(int ppm) { max_co2_ = ppm; }
    void test_setAccuracy(int acc) { accuracy_ = acc; }
    bool test_isStarted() const { return started_; }
    bool test_getAutoCalibration() const { return auto_cal_; }
    bool test_wasZeroCalibrated() const { return zero_calibrated_; }
    int test_getSpan() const { return span_; }
    int test_getRange() const { return range_; }
    bool test_wasRecoveryReset() const { return recovery_reset_; }

private:
    bool started_ = false;
    int co2_ = 400;
    float temperature_ = 24.0f;
    int min_co2_ = 400;
    int max_co2_ = 5000;
    int accuracy_ = 100;
    bool auto_cal_ = true;
    bool zero_calibrated_ = false;
    int span_ = 2000;
    int range_ = 5000;
    bool recovery_reset_ = false;
    byte last_response_[9] = {};
};
