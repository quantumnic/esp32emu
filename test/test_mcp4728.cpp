// test_mcp4728: MCP4728 4-channel DAC mock tests
#include "Adafruit_MCP4728.h"
#include <cassert>
#include <cstdio>

int main() {
    mcp4728_test::reset();
    Adafruit_MCP4728 dac;

    // Init
    assert(dac.begin());

    // Set individual channels
    assert(dac.setChannelValue(MCP4728_CHANNEL_A, 1000));
    assert(mcp4728_test::channels()[0] == 1000);

    assert(dac.setChannelValue(MCP4728_CHANNEL_B, 2000, MCP4728_VREF_INTERNAL, MCP4728_GAIN_2X));
    assert(mcp4728_test::channels()[1] == 2000);
    assert(mcp4728_test::vrefs()[1] == MCP4728_VREF_INTERNAL);
    assert(mcp4728_test::gains()[1] == MCP4728_GAIN_2X);

    // Clamp to 12-bit
    assert(dac.setChannelValue(MCP4728_CHANNEL_C, 5000));
    assert(mcp4728_test::channels()[2] == 4095);

    // Fast write all channels
    assert(dac.fastWrite(100, 200, 300, 400));
    assert(mcp4728_test::channels()[0] == 100);
    assert(mcp4728_test::channels()[1] == 200);
    assert(mcp4728_test::channels()[2] == 300);
    assert(mcp4728_test::channels()[3] == 400);

    // Save to EEPROM
    assert(dac.saveToEEPROM());

    // Unavailable
    mcp4728_test::reset();
    mcp4728_test::available() = false;
    Adafruit_MCP4728 dac2;
    assert(!dac2.begin());

    printf("test_mcp4728: all assertions passed\n");
    return 0;
}
