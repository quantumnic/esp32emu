#include "Adafruit_TSL2591.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_TSL2591 sensor(2591);
    assert(sensor.getSensorID() == 2591);
    assert(sensor.begin());
    assert(sensor.isInitialized());

    // Gain & timing
    sensor.setGain(TSL2591_GAIN_HIGH);
    assert(sensor.getGain() == TSL2591_GAIN_HIGH);
    sensor.setTiming(TSL2591_INTEGRATIONTIME_300MS);
    assert(sensor.getTiming() == TSL2591_INTEGRATIONTIME_300MS);

    // Enable/disable
    sensor.enable();
    sensor.disable();

    // Luminosity
    sensor.setLuminosity(500, 100);
    uint32_t full = sensor.getFullLuminosity();
    uint16_t ir = (full >> 16) & 0xFFFF;
    uint16_t vis = full & 0xFFFF;
    assert(ir == 100);
    assert(vis == 500);

    // Channel reads
    assert(sensor.getLuminosity(0) == 500); // full
    assert(sensor.getLuminosity(1) == 100); // IR
    assert(sensor.getLuminosity(2) == 400); // visible only

    // Lux calculation
    float lux = sensor.calculateLux(500, 100);
    assert(lux > 0);

    // Zero case
    assert(sensor.calculateLux(0, 0) == 0.0f);

    printf("test_tsl2591: all assertions passed\n");
    return 0;
}
