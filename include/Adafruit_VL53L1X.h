#pragma once
// esp32emu: Adafruit VL53L1X Time-of-Flight sensor mock (Adafruit wrapper)
#include <cstdint>
#include <cstdio>

class Adafruit_VL53L1X {
public:
    enum DistanceMode { SHORT = 1, LONG = 2 };

    Adafruit_VL53L1X(uint8_t shutdown_pin = -1, uint8_t irq_pin = -1)
        : _shutdownPin(shutdown_pin), _irqPin(irq_pin) {}

    bool begin(uint8_t i2c_addr = 0x29, void* i2c = nullptr) {
        (void)i2c;
        _addr = i2c_addr;
        _begun = true;
        printf("[esp32emu] Adafruit_VL53L1X: begin (addr=0x%02X)\n", _addr);
        return true;
    }

    // Measurement
    int16_t distance() { return _distance; }
    bool dataReady() { return _begun; }
    void clearInterrupt() {}

    // Start/stop
    bool startRanging() { _ranging = true; return true; }
    bool stopRanging() { _ranging = false; return true; }

    // Configuration
    bool setDistanceMode(DistanceMode mode) { _mode = mode; return true; }
    DistanceMode getDistanceMode() const { return _mode; }

    bool setTimingBudget(uint16_t ms) {
        if (ms != 15 && ms != 20 && ms != 33 && ms != 50 && ms != 100 &&
            ms != 200 && ms != 500) return false;
        _timingBudget = ms;
        return true;
    }
    uint16_t getTimingBudget() const { return _timingBudget; }

    bool setIntermeasurementPeriod(uint16_t ms) { _interPeriod = ms; return true; }
    uint16_t getIntermeasurementPeriod() const { return _interPeriod; }

    // ROI (Region Of Interest)
    bool setROI(uint8_t width, uint8_t height) {
        if (width < 4 || width > 16 || height < 4 || height > 16) return false;
        _roiW = width; _roiH = height;
        return true;
    }
    void getROI(uint8_t* width, uint8_t* height) {
        if (width) *width = _roiW;
        if (height) *height = _roiH;
    }

    bool setROICenter(uint8_t center) { _roiCenter = center; return true; }
    uint8_t getROICenter() { return _roiCenter; }

    // Signal / status
    uint16_t signalRate() { return _signalRate; }
    uint16_t ambientRate() { return _ambientRate; }
    uint8_t rangeStatus() { return _rangeStatus; }

    // Offset/crosstalk calibration
    bool setOffset(int16_t offset) { _offset = offset; return true; }
    int16_t getOffset() { return _offset; }
    bool setXTalk(uint16_t xtalk) { _xtalk = xtalk; return true; }
    uint16_t getXTalk() { return _xtalk; }

    // Test helpers
    void setDistance(int16_t mm) { _distance = mm; }
    void setSignalRate(uint16_t rate) { _signalRate = rate; }
    void setAmbientRate(uint16_t rate) { _ambientRate = rate; }
    void setRangeStatus(uint8_t s) { _rangeStatus = s; }
    bool isBegun() const { return _begun; }
    bool isRanging() const { return _ranging; }

private:
    uint8_t _shutdownPin;
    uint8_t _irqPin;
    uint8_t _addr = 0x29;
    bool _begun = false;
    bool _ranging = false;
    int16_t _distance = 150;
    uint16_t _signalRate = 5000;
    uint16_t _ambientRate = 200;
    uint8_t _rangeStatus = 0;
    DistanceMode _mode = LONG;
    uint16_t _timingBudget = 50;
    uint16_t _interPeriod = 100;
    uint8_t _roiW = 16, _roiH = 16;
    uint8_t _roiCenter = 199;
    int16_t _offset = 0;
    uint16_t _xtalk = 0;
};
