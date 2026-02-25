// esp32emu — test_adafruit_shtc3: SHTC3 humidity/temperature sensor mock test
#include "Adafruit_SHTC3.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_SHTC3 shtc3;

    assert(!shtc3.test_isBegun());
    assert(shtc3.begin());
    assert(shtc3.test_isBegun());

    // Default readings
    sensors_event_t_shtc3 hum, temp;
    assert(shtc3.getEvent(&hum, &temp));
    assert(fabs(temp.temperature - 22.5f) < 0.01f);
    assert(fabs(hum.relative_humidity - 55.0f) < 0.01f);

    // Set custom values
    shtc3.test_setTemperature(30.0f);
    shtc3.test_setHumidity(80.0f);
    assert(shtc3.getEvent(&hum, &temp));
    assert(fabs(temp.temperature - 30.0f) < 0.01f);
    assert(fabs(hum.relative_humidity - 80.0f) < 0.01f);
    assert(fabs(shtc3.test_getTemperature() - 30.0f) < 0.01f);
    assert(fabs(shtc3.test_getHumidity() - 80.0f) < 0.01f);

    // Null pointers OK
    assert(shtc3.getEvent(nullptr, &temp));
    assert(shtc3.getEvent(&hum, nullptr));

    // Sleep/wake
    assert(!shtc3.test_isSleeping());
    assert(shtc3.sleep());
    assert(shtc3.test_isSleeping());
    assert(shtc3.wakeup());
    assert(!shtc3.test_isSleeping());

    // Reset and low power mode
    assert(shtc3.reset());
    assert(shtc3.lowPowerMode(true));

    printf("test_adafruit_shtc3: all assertions passed\n");
    return 0;
}
