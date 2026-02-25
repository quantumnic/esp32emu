#include "Arduino.h"
#include "Adafruit_TSL2561.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    // Test 1: Init
    {
        Adafruit_TSL2561_Unified tsl;
        assert(!tsl.isInitialized());
        assert(tsl.begin());
        assert(tsl.isInitialized());
        assert(tsl.getAddress() == TSL2561_ADDR_FLOAT);
    }

    // Test 2: Custom address
    {
        Adafruit_TSL2561_Unified tsl(TSL2561_ADDR_HIGH, 1234);
        tsl.begin();
        assert(tsl.getAddress() == TSL2561_ADDR_HIGH);
        assert(tsl.getSensorId() == 1234);
    }

    // Test 3: Integration time and gain
    {
        Adafruit_TSL2561_Unified tsl;
        tsl.begin();
        tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);
        tsl.setGain(TSL2561_GAIN_16X);
        assert(tsl.getIntegrationTime() == TSL2561_INTEGRATIONTIME_101MS);
        assert(tsl.getGain() == TSL2561_GAIN_16X);
    }

    // Test 4: Auto range
    {
        Adafruit_TSL2561_Unified tsl;
        tsl.begin();
        assert(!tsl.isAutoRange());
        tsl.enableAutoRange(true);
        assert(tsl.isAutoRange());
    }

    // Test 5: Luminosity read
    {
        Adafruit_TSL2561_Unified tsl;
        tsl.begin();
        tsl.setLuminosity(1000, 200);
        uint16_t bb, ir;
        tsl.getLuminosity(&bb, &ir);
        assert(bb == 1000);
        assert(ir == 200);
    }

    // Test 6: Lux calculation
    {
        Adafruit_TSL2561_Unified tsl;
        tsl.begin();
        uint32_t lux = tsl.calculateLux(500, 100);
        assert(lux > 0);
    }

    // Test 7: Lux with zero broadband
    {
        Adafruit_TSL2561_Unified tsl;
        tsl.begin();
        assert(tsl.calculateLux(0, 0) == 0);
    }

    // Test 8: Sensor event
    {
        Adafruit_TSL2561_Unified tsl;
        tsl.begin();
        tsl.setLuminosity(500, 100);
        Adafruit_TSL2561_Unified::sensor_event_t event;
        assert(tsl.getEvent(&event));
        assert(event.light >= 0);
    }

    // Test 9: Event fails before init
    {
        Adafruit_TSL2561_Unified tsl;
        Adafruit_TSL2561_Unified::sensor_event_t event;
        assert(!tsl.getEvent(&event));
    }

    // Test 10: Enable/disable
    {
        Adafruit_TSL2561_Unified tsl;
        tsl.begin();
        assert(!tsl.isEnabled());
        tsl.enable();
        assert(tsl.isEnabled());
        tsl.disable();
        assert(!tsl.isEnabled());
    }

    // Test 11: setTestLux
    {
        Adafruit_TSL2561_Unified tsl;
        tsl.begin();
        tsl.setTestLux(300.0f);
        uint16_t bb, ir;
        tsl.getLuminosity(&bb, &ir);
        assert(bb > 0);
        assert(ir > 0);
    }

    printf("test_adafruit_tsl2561: all assertions passed\n");
    return 0;
}
