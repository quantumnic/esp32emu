// esp32emu — test_adafruit_mcp4725: MCP4725 DAC mock test
#include "Adafruit_MCP4725.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_MCP4725 dac;

    assert(!dac.test_isBegun());
    assert(dac.begin());
    assert(dac.test_isBegun());
    assert(dac.test_getAddr() == 0x62);

    // Default value
    assert(dac.getValue() == 0);

    // Set voltage
    dac.setVoltage(2048, false);
    assert(dac.getValue() == 2048);
    assert(dac.test_getValue() == 2048);
    float v = dac.test_getVoltage();
    assert(fabs(v - 1.65f) < 0.01f); // ~half of 3.3V

    // Full scale
    dac.setVoltage(4095, false);
    assert(dac.getValue() == 4095);
    assert(fabs(dac.test_getVoltage() - 3.3f) < 0.01f);

    // Zero
    dac.setVoltage(0, false);
    assert(dac.getValue() == 0);
    assert(fabs(dac.test_getVoltage()) < 0.01f);

    // Write with EEPROM
    dac.setVoltage(1000, true);
    assert(dac.getValue() == 1000);

    // Custom Vref
    dac.test_setVref(5.0f);
    dac.setVoltage(4095, false);
    assert(fabs(dac.test_getVoltage() - 5.0f) < 0.01f);

    // Mask to 12 bits
    dac.setVoltage(0xFFFF, false);
    assert(dac.getValue() == 0x0FFF);

    printf("test_adafruit_mcp4725: all assertions passed\n");
    return 0;
}
