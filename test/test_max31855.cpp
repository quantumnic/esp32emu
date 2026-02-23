#include "Adafruit_MAX31855.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    // Software SPI constructor
    Adafruit_MAX31855 tc(13, 10, 12);
    assert(tc.begin());

    // Default temperature
    assert(fabs(tc.readCelsius() - 25.0) < 0.01);
    assert(fabs(tc.readInternal() - 25.0) < 0.01);

    // Set temperature
    tc.test_setTemperature(100.5, 28.0);
    assert(fabs(tc.readCelsius() - 100.5) < 0.01);
    assert(fabs(tc.readFahrenheit() - 212.9) < 0.1);
    assert(fabs(tc.readInternal() - 28.0) < 0.01);
    assert(tc.readError() == 0);

    // Negative temperature
    tc.test_setTemperature(-40.0, 20.0);
    assert(fabs(tc.readCelsius() - (-40.0)) < 0.01);
    assert(fabs(tc.readFahrenheit() - (-40.0)) < 0.1);

    // Error state
    tc.test_setError(Adafruit_MAX31855::MAX31855_FAULT_OPEN);
    assert(std::isnan(tc.readCelsius()));
    assert(tc.readError() == Adafruit_MAX31855::MAX31855_FAULT_OPEN);

    // Hardware SPI constructor
    Adafruit_MAX31855 tc2(5);
    assert(tc2.begin());

    printf("test_max31855: all assertions passed\n");
    return 0;
}
