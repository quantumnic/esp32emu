// test_max6675: MAX6675 thermocouple sensor mock tests
#include "MAX6675.h"
#include <cassert>
#include <cmath>
#include <cstdio>

int main() {
    MAX6675 tc(13, 15, 12); // SCLK, CS, MISO
    assert(tc.getSCLK() == 13);
    assert(tc.getCS() == 15);
    assert(tc.getMISO() == 12);

    // Default temperature
    float c = tc.readCelsius();
    assert(fabs(c - 25.0f) < 0.1f);

    // Set temperature
    tc.setTemperature(100.0f);
    assert(fabs(tc.readCelsius() - 100.0f) < 0.1f);

    // Fahrenheit conversion
    float f = tc.readFahrenheit();
    assert(fabs(f - 212.0f) < 0.1f);

    // Negative temperature
    tc.setTemperature(-40.0f);
    assert(fabs(tc.readCelsius() - (-40.0f)) < 0.1f);
    // -40C == -40F
    assert(fabs(tc.readFahrenheit() - (-40.0f)) < 0.1f);

    // High temperature
    tc.setTemperature(1024.0f);
    assert(fabs(tc.readCelsius() - 1024.0f) < 0.1f);

    printf("test_max6675: all assertions passed\n");
    return 0;
}
