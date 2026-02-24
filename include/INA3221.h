#pragma once
// esp32emu — INA3221 Triple-Channel Power Monitor mock
// TI INA3221 triple-channel, high-side current and bus voltage monitor.

#include <cstdint>
#include <cstdio>
#include <cmath>

class INA3221 {
public:
    static constexpr int NUM_CHANNELS = 3;

    explicit INA3221(uint8_t addr = 0x40) : addr_(addr) {}

    bool begin() {
        initialized_ = true;
        fprintf(stderr, "[esp32emu] INA3221 initialized at 0x%02X\n", addr_);
        return true;
    }

    float getBusVoltage_V(uint8_t ch) const {
        return (ch < NUM_CHANNELS) ? bus_v_[ch] : 0.0f;
    }
    float getShuntVoltage_mV(uint8_t ch) const {
        return (ch < NUM_CHANNELS) ? shunt_mv_[ch] : 0.0f;
    }
    float getCurrent_mA(uint8_t ch) const {
        return (ch < NUM_CHANNELS) ? current_ma_[ch] : 0.0f;
    }
    float getPower_mW(uint8_t ch) const {
        return (ch < NUM_CHANNELS) ? bus_v_[ch] * current_ma_[ch] : 0.0f;
    }

    void enableChannel(uint8_t ch) { if (ch < NUM_CHANNELS) enabled_[ch] = true; }
    void disableChannel(uint8_t ch) { if (ch < NUM_CHANNELS) enabled_[ch] = false; }
    bool isChannelEnabled(uint8_t ch) const { return ch < NUM_CHANNELS && enabled_[ch]; }

    void setShuntResistance(uint8_t ch, float ohms) {
        if (ch < NUM_CHANNELS) shunt_r_[ch] = ohms;
    }
    float getShuntResistance(uint8_t ch) const {
        return (ch < NUM_CHANNELS) ? shunt_r_[ch] : 0.0f;
    }

    void setAveragingMode(uint8_t mode) { avg_mode_ = mode; }
    uint8_t getAveragingMode() const { return avg_mode_; }

    void setPowerMode(uint8_t mode) { power_mode_ = mode; }
    uint8_t getPowerMode() const { return power_mode_; }

    bool isCriticalAlert(uint8_t ch) const { return ch < NUM_CHANNELS && crit_alert_[ch]; }
    bool isWarningAlert(uint8_t ch) const { return ch < NUM_CHANNELS && warn_alert_[ch]; }

    void setCriticalAlertLimit(uint8_t ch, float mv) {
        if (ch < NUM_CHANNELS) crit_limit_[ch] = mv;
    }
    void setWarningAlertLimit(uint8_t ch, float mv) {
        if (ch < NUM_CHANNELS) warn_limit_[ch] = mv;
    }

    uint16_t getManufacturerID() const { return 0x5449; }
    uint16_t getDieID() const { return 0x3220; }

    bool isInitialized() const { return initialized_; }
    uint8_t getAddress() const { return addr_; }

    // ── Test helpers ──
    void setBusVoltage(uint8_t ch, float v) { if (ch < NUM_CHANNELS) bus_v_[ch] = v; }
    void setShuntVoltage(uint8_t ch, float mv) { if (ch < NUM_CHANNELS) shunt_mv_[ch] = mv; }
    void setCurrent(uint8_t ch, float ma) { if (ch < NUM_CHANNELS) current_ma_[ch] = ma; }
    void setReadings(uint8_t ch, float busV, float mA, float shuntR = 0.1f) {
        if (ch < NUM_CHANNELS) {
            bus_v_[ch] = busV; current_ma_[ch] = mA; shunt_mv_[ch] = mA * shuntR;
        }
    }
    void setCriticalAlert(uint8_t ch, bool v) { if (ch < NUM_CHANNELS) crit_alert_[ch] = v; }
    void setWarningAlert(uint8_t ch, bool v) { if (ch < NUM_CHANNELS) warn_alert_[ch] = v; }

private:
    uint8_t addr_;
    bool initialized_ = false;
    uint8_t avg_mode_ = 0;
    uint8_t power_mode_ = 7;  // all channels continuous
    float bus_v_[NUM_CHANNELS] = {5.0f, 3.3f, 12.0f};
    float shunt_mv_[NUM_CHANNELS] = {10.0f, 5.0f, 20.0f};
    float current_ma_[NUM_CHANNELS] = {100.0f, 50.0f, 200.0f};
    float shunt_r_[NUM_CHANNELS] = {0.1f, 0.1f, 0.1f};
    bool enabled_[NUM_CHANNELS] = {true, true, true};
    float crit_limit_[NUM_CHANNELS] = {0, 0, 0};
    float warn_limit_[NUM_CHANNELS] = {0, 0, 0};
    bool crit_alert_[NUM_CHANNELS] = {false, false, false};
    bool warn_alert_[NUM_CHANNELS] = {false, false, false};
};
