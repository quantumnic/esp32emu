#pragma once
// esp32emu — Adafruit INA219 Current/Power Sensor mock
// Adafruit wrapper for TI INA219 high-side current/power monitor.

#include <cstdint>
#include <cstdio>
#include <cmath>

class Adafruit_INA219 {
public:
    explicit Adafruit_INA219(uint8_t addr = 0x40) : addr_(addr) {}

    bool begin() {
        initialized_ = true;
        fprintf(stderr, "[esp32emu] Adafruit_INA219 initialized at 0x%02X\n", addr_);
        return true;
    }

    void setCalibration_32V_2A() { cal_ = 0; }
    void setCalibration_32V_1A() { cal_ = 1; }
    void setCalibration_16V_400mA() { cal_ = 2; }

    float getBusVoltage_V() const { return bus_v_; }
    float getShuntVoltage_mV() const { return shunt_mv_; }
    float getCurrent_mA() const { return current_ma_; }
    float getPower_mW() const { return bus_v_ * current_ma_; }

    int16_t getBusVoltage_raw() const { return (int16_t)(bus_v_ / 0.004f); }
    int16_t getShuntVoltage_raw() const { return (int16_t)(shunt_mv_ / 0.01f); }
    int16_t getCurrent_raw() const { return (int16_t)(current_ma_ / 0.1f); }

    bool success() const { return initialized_; }
    void powerSave(bool on) { ps_ = on; }
    bool isPowerSave() const { return ps_; }

    // ── Test helpers ──
    void setBusVoltage(float v) { bus_v_ = v; }
    void setShuntVoltage(float mv) { shunt_mv_ = mv; }
    void setCurrent(float ma) { current_ma_ = ma; }
    void setReadings(float busV, float mA, float shuntR = 0.1f) {
        bus_v_ = busV; current_ma_ = mA; shunt_mv_ = mA * shuntR;
    }

private:
    uint8_t addr_;
    bool initialized_ = false;
    bool ps_ = false;
    int cal_ = 0;
    float bus_v_ = 5.0f;
    float shunt_mv_ = 10.0f;
    float current_ma_ = 100.0f;
};
