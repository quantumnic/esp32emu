#pragma once
// esp32emu — Unified Adafruit_ADS1X15 mock (ADS1015 + ADS1115)

#include <cstdint>
#include <cstdio>
#include <cmath>

typedef enum {
    GAIN_TWOTHIRDS = 0x0000,  // +/- 6.144V
    GAIN_ONE       = 0x0200,  // +/- 4.096V
    GAIN_TWO       = 0x0400,  // +/- 2.048V
    GAIN_FOUR      = 0x0600,  // +/- 1.024V
    GAIN_EIGHT     = 0x0800,  // +/- 0.512V
    GAIN_SIXTEEN   = 0x0A00,  // +/- 0.256V
} adsGain_t;

class Adafruit_ADS1X15 {
public:
    Adafruit_ADS1X15(int bits) : bits_(bits) {}

    bool begin(uint8_t addr = 0x48, void* /*wire*/ = nullptr) {
        addr_ = addr;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] ADS1X15 (%d-bit): init at 0x%02X\n", bits_, addr);
        return true;
    }

    void setGain(adsGain_t gain) { gain_ = gain; }
    adsGain_t getGain() const { return gain_; }

    int16_t readADC_SingleEnded(uint8_t channel) {
        if (channel > 3) return 0;
        return values_[channel];
    }

    int16_t readADC_Differential_0_1() { return values_[0] - values_[1]; }
    int16_t readADC_Differential_2_3() { return values_[2] - values_[3]; }

    float computeVolts(int16_t counts) const {
        float fsRange;
        switch (gain_) {
            case GAIN_TWOTHIRDS: fsRange = 6.144f; break;
            case GAIN_ONE:       fsRange = 4.096f; break;
            case GAIN_TWO:       fsRange = 2.048f; break;
            case GAIN_FOUR:      fsRange = 1.024f; break;
            case GAIN_EIGHT:     fsRange = 0.512f; break;
            case GAIN_SIXTEEN:   fsRange = 0.256f; break;
            default:             fsRange = 6.144f; break;
        }
        int maxVal = (1 << (bits_ - 1));
        return (float)counts / maxVal * fsRange;
    }

    // Comparator
    void startComparator_SingleEnded(uint8_t channel, int16_t threshold) {
        compChannel_ = channel;
        compThreshold_ = threshold;
    }
    int16_t getLastConversionResults() { return values_[compChannel_]; }

    // Continuous mode
    void startADCReading(uint8_t /*mux*/, bool /*continuous*/) {}
    bool conversionComplete() { return true; }

    // Test helpers
    void test_setValue(uint8_t channel, int16_t val) {
        if (channel < 4) values_[channel] = val;
    }

protected:
    int bits_;
    uint8_t addr_ = 0x48;
    bool initialized_ = false;
    adsGain_t gain_ = GAIN_TWOTHIRDS;
    int16_t values_[4] = {};
    uint8_t compChannel_ = 0;
    int16_t compThreshold_ = 0;
};

class Adafruit_ADS1015 : public Adafruit_ADS1X15 {
public:
    Adafruit_ADS1015() : Adafruit_ADS1X15(12) {}
};

class Adafruit_ADS1115 : public Adafruit_ADS1X15 {
public:
    Adafruit_ADS1115() : Adafruit_ADS1X15(16) {}
};
