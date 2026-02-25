// esp32emu test — Adafruit Si7021 Temperature & Humidity sensor
#include <cassert>
#include <cstdio>
#include <cmath>
#define ESP32EMU_MOCK
#include "Arduino.h"
#include "Adafruit_Si7021.h"

int main() {
    // Test init
    {
        Adafruit_Si7021 si;
        assert(!si.test_isInitialized());
        assert(si.begin());
        assert(si.test_isInitialized());
    }

    // Test default values
    {
        Adafruit_Si7021 si;
        si.begin();
        assert(fabs(si.readTemperature() - 23.5f) < 0.01f);
        assert(fabs(si.readHumidity() - 45.0f) < 0.01f);
    }

    // Test custom values
    {
        Adafruit_Si7021 si;
        si.begin();
        si.test_setTemperature(37.0f);
        si.test_setHumidity(80.0f);
        assert(fabs(si.readTemperature() - 37.0f) < 0.01f);
        assert(fabs(si.readHumidity() - 80.0f) < 0.01f);
    }

    // Test heater
    {
        Adafruit_Si7021 si;
        si.begin();
        assert(!si.isHeaterEnabled());
        si.heater(true);
        assert(si.isHeaterEnabled());
        si.reset();
        assert(!si.isHeaterEnabled());
    }

    // Test resolution
    {
        Adafruit_Si7021 si;
        si.begin();
        assert(si.getResolution() == 0);
        si.setResolution(3);
        assert(si.getResolution() == 3);
    }

    // Test revision
    {
        Adafruit_Si7021 si;
        assert(si.getRevision() == 0x20);
    }

    // Not initialized returns 0
    {
        Adafruit_Si7021 si;
        assert(fabs(si.readTemperature()) < 0.01f);
        assert(fabs(si.readHumidity()) < 0.01f);
    }

    printf("test_si7021: all assertions passed\n");
    return 0;
}
