#include "MCP3008.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.01f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Init
    {
        MCP3008 adc(5);
        assert(!adc.isInitialized());
        assert(adc.begin());
        assert(adc.isInitialized());
        assert(adc.getCSPin() == 5);
    }

    // Test 2: Read channels
    {
        MCP3008 adc;
        adc.begin();
        adc.setADC(0, 512);
        adc.setADC(3, 1023);
        adc.setADC(7, 100);
        assert(adc.readADC(0) == 512);
        assert(adc.readADC(3) == 1023);
        assert(adc.readADC(7) == 100);
        assert(adc.readADC(1) == 0); // unset channel
    }

    // Test 3: Out of range channel
    {
        MCP3008 adc;
        adc.begin();
        assert(adc.readADC(8) == 0);
    }

    // Test 4: Clamp to 1023
    {
        MCP3008 adc;
        adc.begin();
        adc.setADC(0, 2000);
        assert(adc.readADC(0) == 1023);
    }

    // Test 5: Differential reading
    {
        MCP3008 adc;
        adc.begin();
        adc.setADC(0, 800);
        adc.setADC(1, 300);
        assert(adc.readADCDifferential(0, 1) == 500);
        assert(adc.readADCDifferential(1, 0) == -500);
    }

    // Test 6: Voltage conversion
    {
        MCP3008 adc;
        adc.begin();
        // 512 / 1023 * 3.3 ≈ 1.6510
        assert(near(adc.toVoltage(512, 3.3f), 1.651f, 0.01f));
        assert(near(adc.toVoltage(1023, 3.3f), 3.3f, 0.01f));
        assert(near(adc.toVoltage(0, 3.3f), 0.0f));
    }

    // Test 7: 5V reference
    {
        MCP3008 adc;
        adc.begin();
        assert(near(adc.toVoltage(1023, 5.0f), 5.0f, 0.01f));
    }

    // Test 8: getADC helper
    {
        MCP3008 adc;
        adc.begin();
        adc.setADC(4, 777);
        assert(adc.getADC(4) == 777);
        assert(adc.getADC(8) == 0); // out of range
    }

    printf("test_mcp3008: all assertions passed\n");
    return 0;
}
