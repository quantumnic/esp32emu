// esp32emu — test_adafruit_lps22: LPS22 barometric pressure sensor mock test
#include "Adafruit_LPS2X.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_LPS22 lps;

    assert(!lps.test_isBegun());
    assert(lps.begin_I2C());
    assert(lps.test_isBegun());

    // Default readings
    sensors_event_t_lps2x pressure, temp;
    assert(lps.getEvent(&pressure, &temp));
    assert(fabs(pressure.pressure - 1013.25f) < 0.01f);
    assert(fabs(temp.temperature - 22.0f) < 0.01f);

    // Set custom values
    lps.test_setPressure(995.0f);
    lps.test_setTemperature(35.0f);
    assert(lps.getEvent(&pressure, &temp));
    assert(fabs(pressure.pressure - 995.0f) < 0.01f);
    assert(fabs(temp.temperature - 35.0f) < 0.01f);
    assert(fabs(lps.test_getPressure() - 995.0f) < 0.01f);
    assert(fabs(lps.test_getTemperature() - 35.0f) < 0.01f);

    // Null pointers
    assert(lps.getEvent(nullptr, &temp));
    assert(lps.getEvent(&pressure, nullptr));

    // Data rate
    lps.setDataRate(LPS22_RATE_25_HZ);
    assert(lps.getDataRate() == LPS22_RATE_25_HZ);

    // Reset
    lps.reset();

    // LPS25 alias works
    Adafruit_LPS25 lps25;
    assert(lps25.begin_I2C(0x5C));
    assert(lps25.test_isBegun());

    printf("test_adafruit_lps22: all assertions passed\n");
    return 0;
}
