#pragma once
// esp32emu — HDC1080 Temperature & Humidity Sensor mock
// TI HDC1080 low-power, high-accuracy digital humidity/temperature sensor.

#include <cstdint>
#include <cstdio>

class HDC1080 {
public:
    enum Resolution { RES_14BIT = 0, RES_11BIT = 1, RES_8BIT = 2 };

    HDC1080() = default;

    bool begin(uint8_t addr = 0x40) {
        addr_ = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] HDC1080 initialized at 0x%02X\n", addr_);
        return true;
    }

    float readTemperature() const { return temperature_; }
    float readHumidity() const { return humidity_; }

    void setResolution(Resolution tempRes, Resolution humRes) {
        temp_res_ = tempRes;
        hum_res_ = humRes;
    }

    void heatUp(uint8_t seconds) {
        heater_on_ = true;
        fprintf(stderr, "[esp32emu] HDC1080: heater on for %ds\n", seconds);
        heater_on_ = false;
    }

    void turnHeaterOn()  { heater_on_ = true; }
    void turnHeaterOff() { heater_on_ = false; }
    bool isHeaterOn() const { return heater_on_; }

    uint16_t readManufacturerId() const { return 0x5449; }  // "TI"
    uint16_t readDeviceId() const { return 0x1050; }

    float batteryStatus() const { return battery_low_ ? 0.0f : 1.0f; }

    bool isInitialized() const { return initialized_; }
    uint8_t getAddress() const { return addr_; }

    // ── Test helpers ──
    void setTemperature(float t) { temperature_ = t; }
    void setHumidity(float h) { humidity_ = h; }
    void setReadings(float t, float h) { temperature_ = t; humidity_ = h; }
    void setBatteryLow(bool low) { battery_low_ = low; }

private:
    uint8_t addr_ = 0x40;
    bool initialized_ = false;
    bool heater_on_ = false;
    bool battery_low_ = false;
    float temperature_ = 22.5f;
    float humidity_ = 45.0f;
    Resolution temp_res_ = RES_14BIT;
    Resolution hum_res_ = RES_14BIT;
};
