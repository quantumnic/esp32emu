#pragma once
// esp32emu — MCP9600 thermocouple EMF converter mock

#include <cstdint>
#include <cstdio>
#include <cmath>

typedef enum {
    MCP9600_TYPE_K = 0,
    MCP9600_TYPE_J = 1,
    MCP9600_TYPE_T = 2,
    MCP9600_TYPE_N = 3,
    MCP9600_TYPE_S = 4,
    MCP9600_TYPE_E = 5,
    MCP9600_TYPE_B = 6,
    MCP9600_TYPE_R = 7,
} mcp9600_thermocouple_type_t;

typedef enum {
    MCP9600_FILT_NONE = 0,
    MCP9600_FILT_MIN  = 1,
    MCP9600_FILT_MID  = 4,
    MCP9600_FILT_MAX  = 7,
} mcp9600_filter_coeff_t;

typedef enum {
    MCP9600_ADCRES_18BIT = 0,
    MCP9600_ADCRES_16BIT = 1,
    MCP9600_ADCRES_14BIT = 2,
    MCP9600_ADCRES_12BIT = 3,
} mcp9600_adc_resolution_t;

typedef enum {
    MCP9600_ALERT1 = 0,
    MCP9600_ALERT2 = 1,
    MCP9600_ALERT3 = 2,
    MCP9600_ALERT4 = 3,
} mcp9600_alert_num_t;

class MCP9600 {
public:
    MCP9600() = default;

    bool begin(uint8_t addr = 0x67) {
        addr_ = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] MCP9600: initialized at 0x%02X\n", addr);
        return true;
    }

    // Temperature readings
    float readThermocouple() const { return hot_junction_; }
    float readAmbient() const { return cold_junction_; }
    float readADC() const { return adc_raw_; }

    // Delta = hot - cold
    float readDelta() const { return hot_junction_ - cold_junction_; }

    // Configuration
    void setThermocoupleType(mcp9600_thermocouple_type_t type) {
        tc_type_ = type;
        fprintf(stderr, "[esp32emu] MCP9600: thermocouple type=%d\n", (int)type);
    }
    mcp9600_thermocouple_type_t getThermocoupleType() const { return tc_type_; }

    void setFilterCoefficient(mcp9600_filter_coeff_t filt) { filter_ = filt; }
    mcp9600_filter_coeff_t getFilterCoefficient() const { return filter_; }

    void setADCResolution(mcp9600_adc_resolution_t res) { adc_res_ = res; }
    mcp9600_adc_resolution_t getADCResolution() const { return adc_res_; }

    // Alert thresholds
    void setAlertTemperature(mcp9600_alert_num_t alert, float temp) {
        if (alert <= MCP9600_ALERT4) alert_temps_[alert] = temp;
    }
    float getAlertTemperature(mcp9600_alert_num_t alert) const {
        return (alert <= MCP9600_ALERT4) ? alert_temps_[alert] : 0.0f;
    }

    void enableAlert(mcp9600_alert_num_t alert, bool en) {
        if (alert <= MCP9600_ALERT4) alert_enabled_[alert] = en;
    }

    bool alertTriggered(mcp9600_alert_num_t alert) const {
        if (alert > MCP9600_ALERT4 || !alert_enabled_[alert]) return false;
        return hot_junction_ >= alert_temps_[alert];
    }

    uint8_t getDeviceID() const { return 0x40; }
    uint8_t getRevision() const { return 0x01; }

    // Test helpers
    void esp32emu_set_hot_junction(float t) { hot_junction_ = t; }
    void esp32emu_set_cold_junction(float t) { cold_junction_ = t; }
    void esp32emu_set_adc_raw(float v) { adc_raw_ = v; }

private:
    bool initialized_ = false;
    uint8_t addr_ = 0x67;
    mcp9600_thermocouple_type_t tc_type_ = MCP9600_TYPE_K;
    mcp9600_filter_coeff_t filter_ = MCP9600_FILT_NONE;
    mcp9600_adc_resolution_t adc_res_ = MCP9600_ADCRES_18BIT;
    float hot_junction_ = 25.0f;
    float cold_junction_ = 22.0f;
    float adc_raw_ = 0.192f;
    float alert_temps_[4] = {100.0f, 200.0f, 300.0f, 400.0f};
    bool alert_enabled_[4] = {false, false, false, false};
};
