#pragma once
// esp32emu — INA260 Precision Power Monitor mock
// TI INA260 with integrated shunt resistor.

#include <cstdint>
#include <cstdio>

enum class INA260_AveragingCount {
    AVG_1 = 0, AVG_4, AVG_16, AVG_64, AVG_128, AVG_256, AVG_512, AVG_1024
};

enum class INA260_ConversionTime {
    TIME_140_us = 0, TIME_204_us, TIME_332_us, TIME_588_us,
    TIME_1_1_ms, TIME_2_116_ms, TIME_4_156_ms, TIME_8_244_ms
};

enum class INA260_MeasurementMode {
    SHUTDOWN = 0, TRIGGERED = 1, CONTINUOUS = 7
};

class INA260 {
public:
    INA260() = default;

    bool begin(uint8_t addr = 0x40) {
        addr_ = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] INA260 initialized at 0x%02X\n", addr_);
        return true;
    }

    float readBusVoltage() const { return bus_voltage_; }
    float readCurrent() const { return current_ma_; }
    float readPower() const { return power_mw_; }

    void setAveragingCount(INA260_AveragingCount c) { avg_ = c; }
    INA260_AveragingCount getAveragingCount() const { return avg_; }

    void setVoltageConversionTime(INA260_ConversionTime t) { vct_ = t; }
    INA260_ConversionTime getVoltageConversionTime() const { return vct_; }

    void setCurrentConversionTime(INA260_ConversionTime t) { cct_ = t; }
    INA260_ConversionTime getCurrentConversionTime() const { return cct_; }

    void setMode(INA260_MeasurementMode m) { mode_ = m; }
    INA260_MeasurementMode getMode() const { return mode_; }

    bool conversionReady() const { return conversion_ready_; }

    // Alert functions
    void setAlertType(uint8_t type) { alert_type_ = type; }
    void setAlertLimit(float limit) { alert_limit_ = limit; }
    bool alertFunctionFlag() const { return alert_flag_; }

    uint8_t getAddress() const { return addr_; }
    bool isInitialized() const { return initialized_; }

    // ── Test helpers ──
    void setBusVoltage(float v) { bus_voltage_ = v; }
    void setCurrent(float ma) { current_ma_ = ma; }
    void setPower(float mw) { power_mw_ = mw; }
    void setReadings(float v, float ma) {
        bus_voltage_ = v; current_ma_ = ma; power_mw_ = v * ma;
    }
    void setConversionReady(bool r) { conversion_ready_ = r; }
    void setAlertFlag(bool f) { alert_flag_ = f; }

private:
    uint8_t addr_ = 0x40;
    bool initialized_ = false;
    float bus_voltage_ = 5.0f;
    float current_ma_ = 100.0f;
    float power_mw_ = 500.0f;
    INA260_AveragingCount avg_ = INA260_AveragingCount::AVG_1;
    INA260_ConversionTime vct_ = INA260_ConversionTime::TIME_1_1_ms;
    INA260_ConversionTime cct_ = INA260_ConversionTime::TIME_1_1_ms;
    INA260_MeasurementMode mode_ = INA260_MeasurementMode::CONTINUOUS;
    bool conversion_ready_ = true;
    uint8_t alert_type_ = 0;
    float alert_limit_ = 0;
    bool alert_flag_ = false;
};
