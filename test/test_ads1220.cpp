#include "ADS1220.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.001f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Init
    {
        ADS1220 adc;
        assert(!adc.isInitialized());
        assert(adc.begin());
        assert(adc.isInitialized());
    }

    // Test 2: Custom pins
    {
        ADS1220 adc;
        adc.begin(10, 9);
        assert(adc.isInitialized());
    }

    // Test 3: Raw reading
    {
        ADS1220 adc;
        adc.begin();
        adc.setRawValue(0x7FFFFF); // max positive 24-bit
        assert(adc.readRaw() == 0x7FFFFF);
    }

    // Test 4: MUX config
    {
        ADS1220 adc;
        adc.begin();
        adc.setMux(0x05); // AIN1-AIN2 differential
        assert(adc.getMux() == 0x05);
    }

    // Test 5: Gain
    {
        ADS1220 adc;
        adc.begin();
        assert(adc.getGain() == 0); // 1x default
        adc.setGain(3); // 8x
        assert(adc.getGain() == 3);
    }

    // Test 6: Data rate
    {
        ADS1220 adc;
        adc.begin();
        adc.setDataRate(6); // 2000 SPS
        assert(adc.getDataRate() == 6);
    }

    // Test 7: Continuous mode
    {
        ADS1220 adc;
        adc.begin();
        assert(!adc.isContinuousMode());
        adc.setContinuousMode(true);
        assert(adc.isContinuousMode());
    }

    // Test 8: Temperature sensor
    {
        ADS1220 adc;
        adc.begin();
        adc.enableTemperatureSensor(true);
        assert(adc.isTemperatureSensorEnabled());
        adc.setTemperatureC(23.5f);
        assert(near(adc.readTemperature(), 23.5f));
    }

    // Test 9: Power management
    {
        ADS1220 adc;
        adc.begin();
        assert(adc.isPoweredUp());
        adc.powerDown();
        assert(!adc.isPoweredUp());
        adc.powerUp();
        assert(adc.isPoweredUp());
    }

    // Test 10: Data ready
    {
        ADS1220 adc;
        adc.begin();
        assert(adc.isDataReady());
        adc.setDataReady(false);
        assert(!adc.isDataReady());
    }

    // Test 11: Voltage reading
    {
        ADS1220 adc;
        adc.begin();
        adc.setReferenceVoltage(2.048f);
        adc.setGain(0); // 1x
        adc.setRawValue(4194304); // half of 2^23 = 1.024V
        float v = adc.readVoltage();
        assert(near(v, 1.024f, 0.01f));
    }

    printf("test_ads1220: all assertions passed\n");
    return 0;
}
