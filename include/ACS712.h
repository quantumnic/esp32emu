#pragma once
// esp32emu — ACS712 Hall-Effect Current Sensor mock
// Allegro ACS712 analog current sensor (5A/20A/30A variants).

#include <cstdint>
#include <cstdio>
#include <cmath>

class ACS712 {
public:
    enum Type { ACS712_05B = 0, ACS712_20A = 1, ACS712_30A = 2 };

    ACS712(Type type, uint8_t pin) : type_(type), pin_(pin) {
        switch (type_) {
            case ACS712_05B: sensitivity_ = 185.0f; break;
            case ACS712_20A: sensitivity_ = 100.0f; break;
            case ACS712_30A: sensitivity_ = 66.0f;  break;
        }
    }

    void begin() {
        initialized_ = true;
        fprintf(stderr, "[esp32emu] ACS712 initialized on pin %d (sensitivity %.0f mV/A)\n",
                pin_, sensitivity_);
    }

    float getCurrentDC() const { return current_dc_; }
    float getCurrentAC() const { return current_ac_; }
    float getCurrentAC(uint16_t /*freq*/) const { return current_ac_; }

    float getVoltage() const { return midpoint_ + current_dc_ * (sensitivity_ / 1000.0f); }

    void setMidPoint(float v) { midpoint_ = v; }
    float getMidPoint() const { return midpoint_; }
    void calibrate() {
        midpoint_ = 2.5f;
        fprintf(stderr, "[esp32emu] ACS712: calibrated midpoint=%.2fV\n", midpoint_);
    }

    void setSensitivity(float mvPerA) { sensitivity_ = mvPerA; }
    float getSensitivity() const { return sensitivity_; }

    bool isInitialized() const { return initialized_; }
    uint8_t getPin() const { return pin_; }
    Type getType() const { return type_; }

    // ── Test helpers ──
    void setCurrentDC(float a) { current_dc_ = a; }
    void setCurrentAC(float a) { current_ac_ = a; }

private:
    Type type_;
    uint8_t pin_;
    bool initialized_ = false;
    float sensitivity_ = 100.0f;
    float midpoint_ = 2.5f;
    float current_dc_ = 0.0f;
    float current_ac_ = 0.0f;
};
