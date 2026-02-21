#pragma once
#include <cstdint>

// Adafruit MCP9808 precision temperature sensor mock (±0.25°C typical)
// Resolution: 0.0625°C (13-bit), configurable 0.5/0.25/0.125/0.0625°C

#define MCP9808_REG_CONFIG       0x01
#define MCP9808_REG_UPPER_TEMP   0x02
#define MCP9808_REG_LOWER_TEMP   0x03
#define MCP9808_REG_CRIT_TEMP    0x04
#define MCP9808_REG_AMBIENT_TEMP 0x05
#define MCP9808_REG_MANUF_ID     0x06
#define MCP9808_REG_DEVICE_ID    0x07
#define MCP9808_REG_RESOLUTION   0x08

class Adafruit_MCP9808 {
public:
    Adafruit_MCP9808() = default;

    bool begin(uint8_t addr = 0x18) { addr_ = addr; initialized_ = true; return true; }

    float readTempC() { return temperature_c_; }
    float readTempF() { return temperature_c_ * 9.0f / 5.0f + 32.0f; }

    void setResolution(uint8_t res) { resolution_ = res & 0x03; }
    uint8_t getResolution() { return resolution_; }

    void shutdown_wake(bool shutdown) { shutdown_ = shutdown; }
    void shutdown() { shutdown_ = true; }
    void wake() { shutdown_ = false; }

    // Alert thresholds
    void setAlertUpper(float t) { upper_alert_ = t; }
    void setAlertLower(float t) { lower_alert_ = t; }
    void setAlertCritical(float t) { critical_alert_ = t; }
    float getAlertUpper() const { return upper_alert_; }
    float getAlertLower() const { return lower_alert_; }
    float getAlertCritical() const { return critical_alert_; }

    // Test injection
    void setTemperature(float c) { temperature_c_ = c; }

    bool isInitialized() const { return initialized_; }
    uint8_t getAddress() const { return addr_; }
    bool isShutdown() const { return shutdown_; }

private:
    uint8_t addr_ = 0x18;
    bool initialized_ = false;
    bool shutdown_ = false;
    uint8_t resolution_ = 0x03; // max resolution
    float temperature_c_ = 25.0f;
    float upper_alert_ = 35.0f;
    float lower_alert_ = 10.0f;
    float critical_alert_ = 45.0f;
};
