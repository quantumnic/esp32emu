#include "Arduino.h"
#include "Adafruit_TCS34725.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    // Test 1: Init
    {
        Adafruit_TCS34725 tcs;
        assert(!tcs.isInitialized());
        assert(tcs.begin());
        assert(tcs.isInitialized());
    }

    // Test 2: Custom address and config
    {
        Adafruit_TCS34725 tcs(0xEB, 0x02);
        assert(tcs.begin(0x39));
        assert(tcs.getIntegrationTime() == 0xEB);
        assert(tcs.getGain() == 0x02);
        tcs.setIntegrationTime(0xD5);
        tcs.setGain(0x03);
        assert(tcs.getIntegrationTime() == 0xD5);
        assert(tcs.getGain() == 0x03);
    }

    // Test 3: Raw data read
    {
        Adafruit_TCS34725 tcs;
        tcs.begin();
        tcs.setRawData(100, 200, 50, 350);
        uint16_t r, g, b, c;
        tcs.getRawData(&r, &g, &b, &c);
        assert(r == 100);
        assert(g == 200);
        assert(b == 50);
        assert(c == 350);
    }

    // Test 4: RGB calculation
    {
        Adafruit_TCS34725 tcs;
        tcs.begin();
        tcs.setRawData(100, 200, 50, 400);
        float r, g, b;
        tcs.getRGB(&r, &g, &b);
        assert(fabs(r - 63.75f) < 0.5f);   // 100/400*255
        assert(fabs(g - 127.5f) < 0.5f);   // 200/400*255
        assert(fabs(b - 31.875f) < 0.5f);  // 50/400*255
    }

    // Test 5: RGB with zero clear channel
    {
        Adafruit_TCS34725 tcs;
        tcs.begin();
        tcs.setRawData(100, 200, 50, 0);
        float r, g, b;
        tcs.getRGB(&r, &g, &b);
        assert(r == 0 && g == 0 && b == 0);
    }

    // Test 5b: Legacy API compat
    {
        Adafruit_TCS34725 tcs(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
        tcs.begin();
        tcs.setTestColor(1000, 800, 600, 2400);
        uint16_t r, g, b, c;
        assert(tcs.getRawDataOneShot(&r, &g, &b, &c));
        assert(r == 1000 && g == 800);
    }

    // Test 6: Color temperature
    {
        Adafruit_TCS34725 tcs;
        tcs.begin();
        uint16_t ct = tcs.calculateColorTemperature(100, 200, 50);
        assert(ct > 0 && ct < 20000); // sanity
    }

    // Test 7: Lux
    {
        Adafruit_TCS34725 tcs;
        tcs.begin();
        uint16_t lux = tcs.calculateLux(100, 200, 50);
        assert(lux > 0);
    }

    // Test 8: Enable/disable
    {
        Adafruit_TCS34725 tcs;
        tcs.begin();
        assert(!tcs.isEnabled());
        tcs.enable();
        assert(tcs.isEnabled());
        tcs.disable();
        assert(!tcs.isEnabled());
    }

    // Test 9: Interrupt limits
    {
        Adafruit_TCS34725 tcs;
        tcs.begin();
        tcs.setIntLimits(100, 500);
        assert(tcs.getIntLow() == 100);
        assert(tcs.getIntHigh() == 500);
        tcs.setInterrupt(true);
        tcs.clearInterrupt();
    }

    // Test 10: Color temperature dn40 variant
    {
        Adafruit_TCS34725 tcs;
        tcs.begin();
        uint16_t ct = tcs.calculateColorTemperature_dn40(100, 200, 50, 350);
        assert(ct > 0);
    }

    printf("test_tcs34725: all assertions passed\n");
    return 0;
}
