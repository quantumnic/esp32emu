// esp32emu test — Adafruit MAX31856 Precision Thermocouple
#include <cassert>
#include <cstdio>
#include <cmath>
#define ESP32EMU_MOCK
#include "Arduino.h"
#include "Adafruit_MAX31856.h"

int main() {
    // Test init
    {
        Adafruit_MAX31856 tc(10);
        assert(!tc.test_isInitialized());
        assert(tc.begin());
        assert(tc.test_isInitialized());
    }

    // Test default temperatures
    {
        Adafruit_MAX31856 tc(10);
        tc.begin();
        assert(fabs(tc.readThermocoupleTemperature() - 250.0f) < 0.01f);
        assert(fabs(tc.readCJTemperature() - 24.0f) < 0.01f);
    }

    // Test custom temperatures
    {
        Adafruit_MAX31856 tc(10);
        tc.begin();
        tc.test_setThermocoupleTemp(1200.0f);
        tc.test_setCJTemp(28.0f);
        assert(fabs(tc.readThermocoupleTemperature() - 1200.0f) < 0.01f);
        assert(fabs(tc.readCJTemperature() - 28.0f) < 0.01f);
    }

    // Test thermocouple type
    {
        Adafruit_MAX31856 tc(10);
        tc.begin();
        assert(tc.getThermocoupleType() == MAX31856_TCTYPE_K);
        tc.setThermocoupleType(MAX31856_TCTYPE_J);
        assert(tc.getThermocoupleType() == MAX31856_TCTYPE_J);
    }

    // Test conversion mode
    {
        Adafruit_MAX31856 tc(10);
        tc.begin();
        assert(tc.getConversionMode() == MAX31856_ONESHOT);
        tc.setConversionMode(MAX31856_CONTINUOUS);
        assert(tc.getConversionMode() == MAX31856_CONTINUOUS);
    }

    // Test fault
    {
        Adafruit_MAX31856 tc(10);
        tc.begin();
        assert(tc.readFault() == 0);
        tc.test_setFault(0x04);
        assert(tc.readFault() == 0x04);
    }

    // Test one-shot trigger
    {
        Adafruit_MAX31856 tc(10);
        tc.begin();
        assert(!tc.test_oneShotTriggered());
        tc.triggerOneShot();
        assert(tc.test_oneShotTriggered());
        assert(tc.conversionComplete());
    }

    // Not initialized returns 0
    {
        Adafruit_MAX31856 tc(10);
        assert(fabs(tc.readThermocoupleTemperature()) < 0.01f);
    }

    // Test SPI pin constructor
    {
        Adafruit_MAX31856 tc(10, 11, 12, 13);
        assert(tc.begin());
    }

    printf("test_max31856: all assertions passed\n");
    return 0;
}
