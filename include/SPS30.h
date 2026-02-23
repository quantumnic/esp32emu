#pragma once
// esp32emu mock: Sensirion SPS30 — Particulate Matter sensor
#include <cstdint>
#include <cstdio>

struct sps30_measurement {
    float mc_1p0, mc_2p5, mc_4p0, mc_10p0;     // mass concentration µg/m³
    float nc_0p5, nc_1p0, nc_2p5, nc_4p0, nc_10p0; // number concentration #/cm³
    float typical_particle_size;                  // µm
};

class SPS30 {
public:
    SPS30() = default;

    bool begin(void* wire = nullptr) {
        (void)wire;
        _initialized = true;
        printf("[esp32emu] SPS30: initialized\n");
        return true;
    }

    bool start() { _measuring = true; return true; }
    bool stop() { _measuring = false; return true; }

    bool readMeasurement(sps30_measurement* data) {
        if (!_measuring || !data) return false;
        *data = _meas;
        return true;
    }

    bool dataReady() const { return _measuring && _ready; }

    bool clean() { printf("[esp32emu] SPS30: fan cleaning\n"); return true; }
    bool sleep() { _sleeping = true; return true; }
    bool wakeup() { _sleeping = false; return true; }
    bool reset() { _measuring = false; _sleeping = false; return true; }

    bool getSerialNumber(char* buf, uint8_t len) {
        snprintf(buf, len, "SPS30-MOCK-001");
        return true;
    }

    // ── Test helpers ──
    void _test_setMeasurement(const sps30_measurement& m) { _meas = m; _ready = true; }
    void _test_setReady(bool r) { _ready = r; }
    bool _test_isInitialized() const { return _initialized; }
    bool _test_isMeasuring() const { return _measuring; }
    bool _test_isSleeping() const { return _sleeping; }

private:
    bool _initialized = false;
    bool _measuring = false;
    bool _sleeping = false;
    bool _ready = true;
    sps30_measurement _meas{5.0f, 12.0f, 15.0f, 18.0f, 50.0f, 30.0f, 15.0f, 8.0f, 3.0f, 1.5f};
};
