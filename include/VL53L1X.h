#pragma once
// esp32emu: VL53L1X Time-of-Flight distance sensor mock
#include <cstdint>
#include <cstdio>

class VL53L1X {
public:
    enum DistanceMode { Short = 1, Long = 2 };
    enum RangeStatus { RangeValid = 0, SignalFail = 1, MinRangeFail = 2, WrapTargetFail = 3 };

    VL53L1X() : _addr(0x29), _begun(false), _mode(Long), _timingBudget(50000),
        _distance(0), _signalRate(0), _ambientRate(0), _rangeStatus(RangeValid) {}

    bool init(bool /*io_2v8*/ = true) {
        _begun = true;
        printf("[esp32emu] VL53L1X: init (addr=0x%02X)\n", _addr);
        return true;
    }

    void setAddress(uint8_t addr) { _addr = addr; }
    uint8_t getAddress() const { return _addr; }

    void setDistanceMode(DistanceMode mode) { _mode = mode; }
    DistanceMode getDistanceMode() const { return _mode; }

    void setMeasurementTimingBudget(uint32_t budget_us) { _timingBudget = budget_us; }
    uint32_t getMeasurementTimingBudget() const { return _timingBudget; }

    void startContinuous(uint32_t period_ms = 0) { _continuous = true; _period = period_ms; }
    void stopContinuous() { _continuous = false; }

    bool dataReady() { return _begun; }

    uint16_t read(bool /*blocking*/ = true) { return _distance; }
    uint16_t readRangeContinuousMillimeters() { return _distance; }
    uint16_t readRangeSingleMillimeters() { return _distance; }

    uint8_t ranging_data_range_status() { return _rangeStatus; }
    uint16_t ranging_data_peak_signal_count_rate_MCPS() { return _signalRate; }
    uint16_t ranging_data_ambient_count_rate_MCPS() { return _ambientRate; }

    bool timeoutOccurred() { return false; }

    // Test helpers
    void setDistance(uint16_t mm) { _distance = mm; }
    void setSignalRate(uint16_t rate) { _signalRate = rate; }
    void setAmbientRate(uint16_t rate) { _ambientRate = rate; }
    void setRangeStatus(RangeStatus s) { _rangeStatus = s; }
    bool isBegun() const { return _begun; }
    bool isContinuous() const { return _continuous; }

private:
    uint8_t _addr;
    bool _begun;
    DistanceMode _mode;
    uint32_t _timingBudget;
    uint16_t _distance;
    uint16_t _signalRate;
    uint16_t _ambientRate;
    RangeStatus _rangeStatus;
    bool _continuous = false;
    uint32_t _period = 0;
};
