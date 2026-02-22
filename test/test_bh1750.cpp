#include "BH1750.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    BH1750 sensor(0x23);
    assert(!sensor.isBegun());
    assert(sensor.getAddress() == 0x23);

    assert(sensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE));
    assert(sensor.isBegun());
    assert(sensor.getMode() == BH1750::CONTINUOUS_HIGH_RES_MODE);

    // Default lux = 0
    assert(fabs(sensor.readLightLevel()) < 0.01f);

    // Set and read
    sensor.setLux(542.5f);
    assert(fabs(sensor.readLightLevel() - 542.5f) < 0.01f);

    // Configure mode
    sensor.configure(BH1750::ONE_TIME_HIGH_RES_MODE);
    assert(sensor.getMode() == BH1750::ONE_TIME_HIGH_RES_MODE);

    // Measurement ready
    assert(sensor.measurementReady());

    // Alternate address
    BH1750 sensor2(0x5C);
    assert(sensor2.getAddress() == 0x5C);
    assert(sensor2.readLightLevel() < 0); // not begun

    printf("test_bh1750: all assertions passed\n");
    return 0;
}
