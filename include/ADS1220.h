#pragma once
// esp32emu — ADS1220 24-bit Precision ADC mock
// TI ADS1220 4-channel, 24-bit delta-sigma ADC via SPI.

#include <cstdint>
#include <cstdio>

class ADS1220 {
public:
    ADS1220() = default;

    bool begin(uint8_t cs_pin = 5, uint8_t drdy_pin = 4) {
        cs_ = cs_pin;
        drdy_ = drdy_pin;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] ADS1220 initialized (CS=%d, DRDY=%d)\n", cs_, drdy_);
        return true;
    }

    // Read raw 24-bit value from current mux channel
    int32_t readRaw() const { return raw_value_; }

    // Read voltage (based on gain and reference)
    float readVoltage() const {
        float lsb = (vref_ / (1 << 23)) / gain_values_[gain_];
        return raw_value_ * lsb;
    }

    // Channel selection (MUX)
    void setMux(uint8_t mux) { mux_ = mux & 0x0F; }
    uint8_t getMux() const { return mux_; }

    // Gain: 0=1x, 1=2x, 2=4x, 3=8x, 4=16x, 5=32x, 6=64x, 7=128x
    void setGain(uint8_t gain) { gain_ = gain & 0x07; }
    uint8_t getGain() const { return gain_; }

    // Data rate: 0=20SPS .. 6=2000SPS
    void setDataRate(uint8_t rate) { data_rate_ = rate & 0x07; }
    uint8_t getDataRate() const { return data_rate_; }

    // Operating mode
    void setContinuousMode(bool cont) { continuous_ = cont; }
    bool isContinuousMode() const { return continuous_; }

    // Internal temperature sensor
    void enableTemperatureSensor(bool en) { temp_sensor_ = en; }
    bool isTemperatureSensorEnabled() const { return temp_sensor_; }
    float readTemperature() const { return temp_c_; }

    // Reference voltage
    void setVRef(uint8_t ref) { vref_sel_ = ref & 0x03; }
    uint8_t getVRef() const { return vref_sel_; }

    // Power-down
    void powerDown() { powered_ = false; }
    void powerUp() { powered_ = true; }
    bool isPoweredUp() const { return powered_; }

    // Data ready
    bool isDataReady() const { return data_ready_; }

    bool isInitialized() const { return initialized_; }

    // ── Test helpers ──
    void setRawValue(int32_t raw) { raw_value_ = raw; }
    void setTemperatureC(float t) { temp_c_ = t; }
    void setDataReady(bool rdy) { data_ready_ = rdy; }
    void setReferenceVoltage(float v) { vref_ = v; }

private:
    uint8_t cs_ = 5;
    uint8_t drdy_ = 4;
    bool initialized_ = false;
    bool continuous_ = false;
    bool temp_sensor_ = false;
    bool powered_ = true;
    bool data_ready_ = true;
    uint8_t mux_ = 0;
    uint8_t gain_ = 0;
    uint8_t data_rate_ = 4; // 330 SPS default
    uint8_t vref_sel_ = 0;
    int32_t raw_value_ = 0;
    float temp_c_ = 25.0f;
    float vref_ = 2.048f;

    static constexpr float gain_values_[8] = {1, 2, 4, 8, 16, 32, 64, 128};
};

constexpr float ADS1220::gain_values_[8];
