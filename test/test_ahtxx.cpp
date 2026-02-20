#include "AHTxx.h"
#include <cassert>
#include <cmath>
#include <cstdio>

int main() {
    AHTxx sensor(AHT20_ADDRESS, AHT2x_SENSOR);
    assert(!sensor.isInitialized());

    // Before init, returns error values
    assert(sensor.readTemperature() == 255.0f);
    assert(sensor.readHumidity() == 255.0f);

    assert(sensor.begin());
    assert(sensor.isInitialized());
    assert(sensor.getStatus() == 0x18);

    // Default readings
    assert(std::fabs(sensor.readTemperature() - 23.0f) < 0.01f);
    assert(std::fabs(sensor.readHumidity() - 50.0f) < 0.01f);

    // Set values
    sensor.setTemperature(-5.0f);
    sensor.setHumidity(95.0f);
    assert(std::fabs(sensor.readTemperature() - (-5.0f)) < 0.01f);
    assert(std::fabs(sensor.readHumidity() - 95.0f) < 0.01f);

    // Soft reset
    assert(sensor.softReset());

    // AHT10 alias works
    AHT10 aht10;
    assert(aht10.begin());

    // AHT20 alias works
    AHT20 aht20;
    assert(aht20.begin());

    printf("test_ahtxx: all assertions passed\n");
    return 0;
}
