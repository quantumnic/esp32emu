#include "MAX31856.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_MAX31856 tc(5); // CS pin 5
    assert(tc.begin());

    // Default type K
    assert(tc.getThermocoupleType() == MAX31856_TCTYPE_K);
    tc.setThermocoupleType(MAX31856_TCTYPE_J);
    assert(tc.getThermocoupleType() == MAX31856_TCTYPE_J);

    // Default temperatures
    assert(std::fabs(tc.readThermocoupleTemperature() - 250.0f) < 0.01f);
    assert(std::fabs(tc.readCJTemperature() - 25.0f) < 0.01f);

    // Test helpers
    tc.esp32emu_set_tc_temperature(1200.5f);
    tc.esp32emu_set_cj_temperature(28.3f);
    assert(std::fabs(tc.readThermocoupleTemperature() - 1200.5f) < 0.01f);
    assert(std::fabs(tc.readCJTemperature() - 28.3f) < 0.01f);

    // Fault
    assert(tc.readFault() == 0);
    tc.esp32emu_set_fault(0x42);
    assert(tc.readFault() == 0x42);

    // One-shot conversion
    tc.setConversionMode(true);
    tc.triggerOneShot();
    assert(tc.conversionComplete());

    printf("test_max31856: all assertions passed\n");
    return 0;
}
