#include "DPS310.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Dps310 sensor;

    assert(sensor.begin() == 0);
    assert(sensor._test_isInitialized());

    // Default readings
    float temp, prs;
    assert(sensor.measureTempOnce(temp) == 0);
    assert(fabsf(temp - 22.0f) < 0.01f);

    assert(sensor.measurePressureOnce(prs) == 0);
    assert(fabsf(prs - 1013.25f) < 0.01f);

    // Inject values
    sensor._test_setTemperature(30.0f);
    sensor._test_setPressure(980.0f);
    assert(sensor.measureTempOnce(temp) == 0);
    assert(fabsf(temp - 30.0f) < 0.01f);
    assert(sensor.measurePressureOnce(prs) == 0);
    assert(fabsf(prs - 980.0f) < 0.01f);

    // Altitude calculation (at 980 hPa, sea level 1013.25 hPa → ~280m)
    float alt = sensor.getAltitude();
    assert(alt > 200.0f && alt < 350.0f);

    // Standby
    assert(sensor.standby() == 0);
    assert(sensor._test_isStandby());

    printf("test_dps310: all assertions passed\n");
    return 0;
}
