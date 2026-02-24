// esp32emu test — Adafruit CAP1188 Capacitive Touch Sensor
#include <cassert>
#include <cstdio>
#define ESP32EMU_MOCK
#include "Arduino.h"
#include "Adafruit_CAP1188.h"

int main() {
    // Test init
    {
        Adafruit_CAP1188 cap;
        assert(cap.begin());
        assert(cap.readProductID() == 0x50);
        assert(cap.readManufacturerID() == 0x5D);
        assert(cap.readRevision() == 0x83);
    }

    // Test touch reading
    {
        Adafruit_CAP1188 cap;
        cap.begin();
        assert(cap.touched() == 0);
        cap.test_setTouched(0b00001010);
        assert(cap.touched() == 0x0A);
    }

    // Test delta reading
    {
        Adafruit_CAP1188 cap;
        cap.begin();
        cap.test_setDelta(0, 42);
        cap.test_setDelta(7, -10);
        assert(cap.readDelta(0) == 42);
        assert(cap.readDelta(7) == -10);
        assert(cap.readDelta(8) == 0); // out of range
    }

    // Test sensitivity
    {
        Adafruit_CAP1188 cap;
        cap.begin();
        cap.setSensitivity(64);
        assert(cap.getSensitivity() == 64);
    }

    // Test multi-touch
    {
        Adafruit_CAP1188 cap;
        cap.begin();
        cap.setMultiTouch(true);
        cap.test_setTouched(0xFF);
        assert(cap.touched() == 0xFF);
    }

    // Test custom address and reset pin
    {
        Adafruit_CAP1188 cap(4);
        assert(cap.begin(0x28));
    }

    printf("test_adafruit_cap1188: all assertions passed\n");
    return 0;
}
