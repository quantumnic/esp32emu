#pragma once
// esp32emu: Adafruit SGP30 air quality sensor mock (eCO2 + TVOC)
#include "Arduino.h"
#include "Wire.h"
#include <cstdint>
#include <cstdio>

class Adafruit_SGP30 {
public:
    uint16_t TVOC = 0;
    uint16_t eCO2 = 400;
    uint16_t rawH2 = 13000;
    uint16_t rawEthanol = 18000;

    uint32_t serialnumber[3] = {0x0000, 0x0048, 0xB6E0};

    Adafruit_SGP30() = default;

    bool begin(TwoWire* wire = &Wire) {
        (void)wire;
        _begun = true;
        printf("[esp32emu] SGP30: initialized\n");
        return true;
    }

    bool IAQinit() {
        _iaqInit = true;
        return true;
    }

    bool IAQmeasure() {
        if (!_begun) return false;
        TVOC = _tvoc;
        eCO2 = _eco2;
        _measureCount++;
        return true;
    }

    bool IAQmeasureRaw() {
        if (!_begun) return false;
        rawH2 = _rawH2;
        rawEthanol = _rawEthanol;
        return true;
    }

    bool getIAQBaseline(uint16_t* eco2_base, uint16_t* tvoc_base) {
        *eco2_base = _eco2Baseline;
        *tvoc_base = _tvocBaseline;
        return true;
    }

    bool setIAQBaseline(uint16_t eco2_base, uint16_t tvoc_base) {
        _eco2Baseline = eco2_base;
        _tvocBaseline = tvoc_base;
        return true;
    }

    bool setHumidity(uint32_t absolute_humidity) {
        _absHumidity = absolute_humidity;
        return true;
    }

    // ── Test helpers ──
    void emu_setTVOC(uint16_t v) { _tvoc = v; }
    void emu_seteCO2(uint16_t v) { _eco2 = v; }
    void emu_setRawH2(uint16_t v) { _rawH2 = v; }
    void emu_setRawEthanol(uint16_t v) { _rawEthanol = v; }
    bool emu_begun() const { return _begun; }
    bool emu_iaqInit() const { return _iaqInit; }
    uint32_t emu_measureCount() const { return _measureCount; }
    uint32_t emu_absHumidity() const { return _absHumidity; }

private:
    bool _begun = false;
    bool _iaqInit = false;
    uint16_t _tvoc = 0;
    uint16_t _eco2 = 400;
    uint16_t _rawH2 = 13000;
    uint16_t _rawEthanol = 18000;
    uint16_t _eco2Baseline = 0;
    uint16_t _tvocBaseline = 0;
    uint32_t _absHumidity = 0;
    uint32_t _measureCount = 0;
};
