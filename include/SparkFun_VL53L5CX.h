#pragma once
#include <cstdint>
#include <cstring>
#include <cstdio>

// 8x8 multizone ToF sensor
#define VL53L5CX_RESOLUTION_4X4  16
#define VL53L5CX_RESOLUTION_8X8  64
#define VL53L5CX_STATUS_VALID    5

struct VL53L5CX_ResultsData {
    int16_t distance_mm[64];
    uint8_t target_status[64];
    uint8_t nb_target_detected[64];
    int16_t signal_per_spad[64];
    int16_t ambient_per_spad[64];
};

class SparkFun_VL53L5CX {
public:
    SparkFun_VL53L5CX() { memset(&_results, 0, sizeof(_results)); }

    bool begin() {
        _started = true;
        // Default: 4x4 with reasonable distances
        for (int i = 0; i < 64; i++) {
            _results.distance_mm[i] = 500 + (i * 10);
            _results.target_status[i] = VL53L5CX_STATUS_VALID;
            _results.nb_target_detected[i] = 1;
            _results.signal_per_spad[i] = 2000;
            _results.ambient_per_spad[i] = 100;
        }
        return true;
    }

    bool setResolution(uint8_t resolution) {
        if (resolution != VL53L5CX_RESOLUTION_4X4 && resolution != VL53L5CX_RESOLUTION_8X8)
            return false;
        _resolution = resolution;
        return true;
    }
    uint8_t getResolution() const { return _resolution; }

    bool setRangingFrequency(uint8_t hz) { _frequency = hz; return true; }
    uint8_t getRangingFrequency() const { return _frequency; }

    bool setRangingMode(uint8_t mode) { _mode = mode; return true; }
    uint8_t getRangingMode() const { return _mode; }

    bool setIntegrationTime(uint32_t ms) { _integration_ms = ms; return true; }
    uint32_t getIntegrationTime() const { return _integration_ms; }

    bool setSharpenerPercent(uint8_t pct) { _sharpener = pct; return true; }
    uint8_t getSharpenerPercent() const { return _sharpener; }

    bool startRanging() { _ranging = true; return true; }
    bool stopRanging() { _ranging = false; return true; }
    bool isDataReady() const { return _data_ready; }

    bool getRangingData(VL53L5CX_ResultsData* results) {
        if (!results) return false;
        *results = _results;
        _data_ready = false;
        return true;
    }

    // Test helpers
    void _test_set_data_ready(bool ready) { _data_ready = ready; }
    void _test_set_distance(uint8_t zone, int16_t mm) {
        if (zone < 64) _results.distance_mm[zone] = mm;
    }
    void _test_set_results(const VL53L5CX_ResultsData& r) { _results = r; _data_ready = true; }
    VL53L5CX_ResultsData& _test_get_results() { return _results; }

private:
    bool _started = false;
    bool _ranging = false;
    bool _data_ready = true;
    uint8_t _resolution = VL53L5CX_RESOLUTION_4X4;
    uint8_t _frequency = 15;
    uint8_t _mode = 1;  // continuous
    uint32_t _integration_ms = 5;
    uint8_t _sharpener = 5;
    VL53L5CX_ResultsData _results;
};
