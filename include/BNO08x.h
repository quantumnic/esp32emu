#pragma once
// esp32emu mock: BNO08x (Hillcrest BNO085/BNO080) — 9-DOF IMU with sensor fusion
#include <cstdint>
#include <cstdio>
#include <cmath>

// Report IDs
#define SH2_ACCELEROMETER          0x01
#define SH2_GYROSCOPE_CALIBRATED   0x02
#define SH2_MAGNETIC_FIELD         0x03
#define SH2_ROTATION_VECTOR        0x05
#define SH2_GAME_ROTATION_VECTOR   0x08
#define SH2_STEP_COUNTER           0x11
#define SH2_STABILITY_CLASSIFIER   0x13
#define SH2_SHAKE_DETECTOR         0x19

struct sh2_SensorValue_t {
    uint8_t sensorId;
    uint8_t status;
    uint64_t timestamp;
    union {
        struct { float x, y, z; } un_accelerometer;
        struct { float x, y, z; } un_gyroscope;
        struct { float x, y, z; } un_magneticField;
        struct { float i, j, k, real; float accuracy; } un_rotationVector;
        struct { float i, j, k, real; } un_gameRotationVector;
        struct { uint32_t steps; } un_stepCounter;
        struct { uint8_t classification; } un_stabilityClassifier;
    };
};

class BNO08x {
public:
    BNO08x(int resetPin = -1) { (void)resetPin; }

    bool begin_I2C(uint8_t addr = 0x4A, void* wire = nullptr) {
        (void)addr; (void)wire;
        _initialized = true;
        printf("[esp32emu] BNO08x: initialized (I2C)\n");
        return true;
    }

    bool begin_SPI(int csPin, int intPin, int rstPin = -1) {
        (void)csPin; (void)intPin; (void)rstPin;
        _initialized = true;
        printf("[esp32emu] BNO08x: initialized (SPI)\n");
        return true;
    }

    bool enableReport(uint8_t sensorId, uint32_t interval_us = 10000) {
        (void)interval_us;
        _enabledReports |= (1u << sensorId);
        return true;
    }

    bool wasReset() { bool r = _wasReset; _wasReset = false; return r; }

    bool getSensorEvent(sh2_SensorValue_t* value) {
        if (!_initialized || !_dataReady || !value) return false;
        *value = _sensorValue;
        _dataReady = false;
        return true;
    }

    // ── Test helpers ──
    void _test_injectEvent(const sh2_SensorValue_t& val) {
        _sensorValue = val;
        _dataReady = true;
    }
    void _test_setReset(bool r) { _wasReset = r; }
    bool _test_isInitialized() const { return _initialized; }
    bool _test_reportEnabled(uint8_t id) const { return (_enabledReports >> id) & 1; }

private:
    bool _initialized = false;
    bool _dataReady = false;
    bool _wasReset = false;
    uint32_t _enabledReports = 0;
    sh2_SensorValue_t _sensorValue{};
};
