#include "Adafruit_AHTX0.h"
#include <cassert>
#include <cmath>
#include <cstdio>

int main() {
    Adafruit_AHTX0 aht;
    assert(!aht._isInitialized());

    // getEvent before init fails
    Adafruit_AHTX0::sensors_event_t h, t;
    assert(!aht.getEvent(&h, &t));

    // Init
    assert(aht.begin());
    assert(aht._isInitialized());
    assert(aht.getStatus() == 0x18);

    // Default values
    assert(aht.getEvent(&h, &t));
    assert(std::fabs(t.temperature - 23.0f) < 0.01f);
    assert(std::fabs(h.relative_humidity - 50.0f) < 0.01f);

    // Set values
    aht._setTemperature(31.5f);
    aht._setHumidity(75.0f);
    assert(aht.getEvent(&h, &t));
    assert(std::fabs(t.temperature - 31.5f) < 0.01f);
    assert(std::fabs(h.relative_humidity - 75.0f) < 0.01f);

    // Null pointers ok
    assert(aht.getEvent(nullptr, &t));
    assert(aht.getEvent(&h, nullptr));
    assert(aht.getEvent(nullptr, nullptr));

    printf("test_ahtx0: all assertions passed\n");
    return 0;
}
