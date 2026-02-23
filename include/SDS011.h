#pragma once
#include <cstdint>

// SDS011 Nova PM2.5/PM10 sensor mock
class SDS011 {
public:
    SDS011() = default;

    void begin(HardwareSerial& /*serial*/) { started_ = true; }
    // Alternate begin with pin numbers
    void begin(int rxPin, int txPin) {
        (void)rxPin; (void)txPin;
        started_ = true;
    }

    int read(float *pm25, float *pm10) {
        if (!started_) return 1;
        if (pm25) *pm25 = pm25_;
        if (pm10) *pm10 = pm10_;
        return error_code_;
    }

    void sleep() { sleeping_ = true; }
    void wakeup() { sleeping_ = false; }

    void setWorkingPeriod(uint8_t minutes) { working_period_ = minutes; }
    uint8_t getWorkingPeriod() const { return working_period_; }

    void setQueryMode() { query_mode_ = true; }
    void setActiveMode() { query_mode_ = false; }
    bool isQueryMode() const { return query_mode_; }

    String firmwareVersion() { return firmware_; }

    // Test helpers
    void test_setPM25(float v) { pm25_ = v; }
    void test_setPM10(float v) { pm10_ = v; }
    void test_setErrorCode(int e) { error_code_ = e; }
    void test_setFirmware(const String& fw) { firmware_ = fw; }
    bool test_isStarted() const { return started_; }
    bool test_isSleeping() const { return sleeping_; }

private:
    bool started_ = false;
    bool sleeping_ = false;
    bool query_mode_ = false;
    float pm25_ = 12.5f;
    float pm10_ = 25.0f;
    int error_code_ = 0;
    uint8_t working_period_ = 0;
    String firmware_ = "19.11";
};
