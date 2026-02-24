#pragma once
#include <cstdint>
#include <cstdio>

// Sensirion SCD4x CO2, temperature, humidity sensor mock (SCD40/SCD41)
class SensirionI2CScd4x {
public:
    SensirionI2CScd4x() = default;

    void begin(TwoWire& /*wire*/) { started_ = true; }

    uint16_t startPeriodicMeasurement() { measuring_ = true; return 0; }
    uint16_t stopPeriodicMeasurement() { measuring_ = false; return 0; }

    uint16_t startLowPowerPeriodicMeasurement() { measuring_ = true; low_power_ = true; return 0; }

    uint16_t readMeasurement(uint16_t& co2, float& temperature, float& humidity) {
        if (!data_ready_) return 1;
        co2 = co2_;
        temperature = temperature_;
        humidity = humidity_;
        return 0;
    }

    uint16_t getDataReadyFlag(bool& dataReady) {
        dataReady = data_ready_;
        return 0;
    }

    uint16_t setTemperatureOffset(float offset) { temp_offset_ = offset; return 0; }
    uint16_t getTemperatureOffset(float& offset) { offset = temp_offset_; return 0; }

    uint16_t setSensorAltitude(uint16_t altitude) { altitude_ = altitude; return 0; }
    uint16_t getSensorAltitude(uint16_t& altitude) { altitude = altitude_; return 0; }

    uint16_t setAmbientPressure(uint16_t pressure) { ambient_pressure_ = pressure; return 0; }

    uint16_t performForcedRecalibration(uint16_t targetCo2, uint16_t& frcCorrection) {
        frc_target_ = targetCo2;
        frcCorrection = 0x8000 + (targetCo2 - co2_);
        forced_recal_ = true;
        return 0;
    }

    uint16_t setAutomaticSelfCalibration(uint16_t enabled) { asc_enabled_ = (enabled != 0); return 0; }
    uint16_t getAutomaticSelfCalibration(uint16_t& enabled) { enabled = asc_enabled_ ? 1 : 0; return 0; }

    uint16_t persistSettings() { persisted_ = true; return 0; }
    uint16_t performFactoryReset() {
        co2_ = 400; temperature_ = 25.0f; humidity_ = 50.0f;
        temp_offset_ = 4.0f; altitude_ = 0; asc_enabled_ = true;
        return 0;
    }
    uint16_t reinit() { return 0; }

    uint16_t getSerialNumber(uint16_t& s0, uint16_t& s1, uint16_t& s2) {
        s0 = 0xBEEF; s1 = 0xCAFE; s2 = 0x1234;
        return 0;
    }

    uint16_t measureSingleShot() { single_shot_ = true; data_ready_ = true; return 0; }
    uint16_t measureSingleShotRhtOnly() { single_shot_rht_ = true; data_ready_ = true; return 0; }

    uint16_t powerDown() { powered_down_ = true; return 0; }
    uint16_t wakeUp() { powered_down_ = false; return 0; }

    // Test helpers
    void test_setCO2(uint16_t ppm) { co2_ = ppm; }
    void test_setTemperature(float t) { temperature_ = t; }
    void test_setHumidity(float h) { humidity_ = h; }
    void test_setDataReady(bool ready) { data_ready_ = ready; }
    bool test_isStarted() const { return started_; }
    bool test_isMeasuring() const { return measuring_; }
    bool test_isLowPower() const { return low_power_; }
    bool test_isPoweredDown() const { return powered_down_; }
    bool test_wasForcedRecal() const { return forced_recal_; }
    bool test_wasPersisted() const { return persisted_; }
    uint16_t test_getFrcTarget() const { return frc_target_; }
    float test_getTempOffset() const { return temp_offset_; }
    uint16_t test_getAltitude() const { return altitude_; }
    uint16_t test_getAmbientPressure() const { return ambient_pressure_; }

private:
    bool started_ = false;
    bool measuring_ = false;
    bool low_power_ = false;
    bool data_ready_ = false;
    bool powered_down_ = false;
    bool asc_enabled_ = true;
    bool forced_recal_ = false;
    bool persisted_ = false;
    bool single_shot_ = false;
    bool single_shot_rht_ = false;
    uint16_t co2_ = 400;
    float temperature_ = 25.0f;
    float humidity_ = 50.0f;
    float temp_offset_ = 4.0f;
    uint16_t altitude_ = 0;
    uint16_t ambient_pressure_ = 0;
    uint16_t frc_target_ = 0;
};
