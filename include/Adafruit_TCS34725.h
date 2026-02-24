#pragma once
// esp32emu: Adafruit TCS34725 color sensor mock — I2C RGBC color detection
#include <cstdint>
#include <cstdio>
#include <cmath>

// Integration time constants
#define TCS34725_INTEGRATIONTIME_2_4MS  0xFF
#define TCS34725_INTEGRATIONTIME_24MS   0xF6
#define TCS34725_INTEGRATIONTIME_50MS   0xEB
#define TCS34725_INTEGRATIONTIME_101MS  0xD5
#define TCS34725_INTEGRATIONTIME_154MS  0xC0
#define TCS34725_INTEGRATIONTIME_700MS  0x00

// Gain constants
#define TCS34725_GAIN_1X   0x00
#define TCS34725_GAIN_4X   0x01
#define TCS34725_GAIN_16X  0x02
#define TCS34725_GAIN_60X  0x03

class Adafruit_TCS34725 {
public:
    Adafruit_TCS34725(uint8_t integrationTime = 0xFF, uint8_t gain = 0x01)
        : integrationTime_(integrationTime), gain_(gain) {}

    bool begin(uint8_t addr = 0x29) {
        addr_ = addr;
        initialized_ = true;
        printf("[esp32emu] TCS34725: initialized at 0x%02X (itime=0x%02X, gain=0x%02X)\n",
               addr, integrationTime_, gain_);
        return true;
    }

    void setIntegrationTime(uint8_t it) { integrationTime_ = it; }
    void setGain(uint8_t gain) { gain_ = gain; }

    void getRawData(uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c) {
        *r = rawR_; *g = rawG_; *b = rawB_; *c = rawC_;
    }

    void getRGB(float* r, float* g, float* b) {
        if (rawC_ == 0) { *r = *g = *b = 0; return; }
        *r = (float)rawR_ / rawC_ * 255.0f;
        *g = (float)rawG_ / rawC_ * 255.0f;
        *b = (float)rawB_ / rawC_ * 255.0f;
    }

    uint16_t calculateColorTemperature(uint16_t r, uint16_t g, uint16_t b) {
        // McCamy's formula approximation
        float X = (-0.14282f * r) + (1.54924f * g) + (-0.95641f * b);
        float Y = (-0.32466f * r) + (1.57837f * g) + (-0.73191f * b);
        float Z = (-0.68202f * r) + (0.77073f * g) + (0.56332f * b);
        float xc = X / (X + Y + Z);
        float yc = Y / (X + Y + Z);
        float n = (xc - 0.3320f) / (0.1858f - yc);
        float cct = (449.0f * n * n * n) + (3525.0f * n * n) + (6823.3f * n) + 5520.33f;
        return static_cast<uint16_t>(cct);
    }

    uint16_t calculateColorTemperature_dn40(uint16_t r, uint16_t g, uint16_t b, uint16_t c) {
        (void)c;
        return calculateColorTemperature(r, g, b);
    }

    uint16_t calculateLux(uint16_t r, uint16_t g, uint16_t b) {
        float illuminance = (-0.32466f * r) + (1.57837f * g) + (-0.73191f * b);
        return static_cast<uint16_t>(illuminance > 0 ? illuminance : 0);
    }

    void enable() { enabled_ = true; }
    void disable() { enabled_ = false; }

    void setInterrupt(bool flag) { interrupt_ = flag; }
    void clearInterrupt() { interrupt_ = false; }
    void setIntLimits(uint16_t low, uint16_t high) { intLow_ = low; intHigh_ = high; }

    // Test helpers
    // Legacy aliases
    void setTestColor(uint16_t r, uint16_t g, uint16_t b, uint16_t c) { setRawData(r,g,b,c); }
    bool getRawDataOneShot(uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c) { getRawData(r,g,b,c); return true; }

    void setRawData(uint16_t r, uint16_t g, uint16_t b, uint16_t c) {
        rawR_ = r; rawG_ = g; rawB_ = b; rawC_ = c;
    }
    bool isInitialized() const { return initialized_; }
    bool isEnabled() const { return enabled_; }
    uint8_t getGain() const { return gain_; }
    uint8_t getIntegrationTime() const { return integrationTime_; }
    uint16_t getIntLow() const { return intLow_; }
    uint16_t getIntHigh() const { return intHigh_; }

private:
    uint8_t addr_ = 0x29;
    uint8_t integrationTime_;
    uint8_t gain_;
    bool initialized_ = false;
    bool enabled_ = false;
    bool interrupt_ = false;
    uint16_t rawR_ = 200, rawG_ = 180, rawB_ = 150, rawC_ = 530;
    uint16_t intLow_ = 0, intHigh_ = 0xFFFF;
};
