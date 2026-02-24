// esp32emu test — PCF8575 16-bit I2C I/O Expander
#include <cassert>
#include <cstdio>
#define ESP32EMU_MOCK
#include "Arduino.h"
#include "PCF8575.h"

int main() {
    // Test construction and init
    {
        PCF8575 expander(0x20);
        assert(!expander.isConnected());
        assert(expander.begin());
        assert(expander.isConnected());
        assert(expander.getAddress() == 0x20);
    }

    // Test write16/read16
    {
        PCF8575 expander(0x21);
        expander.begin();
        assert(expander.write16(0xABCD));
        assert(expander.read16() == 0xABCD);
        assert(expander.write16(0x0000));
        assert(expander.read16() == 0x0000);
    }

    // Test single pin write/read (pins 0-15)
    {
        PCF8575 expander;
        expander.begin();
        expander.write16(0x0000);
        assert(expander.write(12, HIGH));
        assert(expander.read(12) == 1);
        assert(expander.read(0) == 0);
        assert(expander.write(12, LOW));
        assert(expander.read(12) == 0);
    }

    // Test high pins (8-15)
    {
        PCF8575 expander;
        expander.begin();
        expander.write16(0x0000);
        assert(expander.write(15, HIGH));
        assert(expander.read16() == 0x8000);
        assert(expander.write(8, HIGH));
        assert(expander.read16() == 0x8100);
    }

    // Test invalid pin
    {
        PCF8575 expander;
        expander.begin();
        assert(!expander.write(16, HIGH));
        assert(expander.read(16) == 0);
        assert(!expander.toggle(16));
    }

    // Test toggle
    {
        PCF8575 expander;
        expander.begin();
        expander.write16(0x0000);
        assert(expander.toggle(0));
        assert(expander.read(0) == 1);
        assert(expander.toggle(0));
        assert(expander.read(0) == 0);
        // Toggle high byte pin
        assert(expander.toggle(15));
        assert(expander.read(15) == 1);
        assert(expander.read16() == 0x8000);
    }

    // Test helpers
    {
        PCF8575 expander;
        expander.begin();
        expander.test_setInput(0b1010101010101010);
        assert(expander.read16() == 0xAAAA);
        assert(expander.read(1) == 1);
        assert(expander.read(0) == 0);
        expander.test_setPin(0, true);
        assert(expander.read(0) == 1);
        assert(expander.test_getOutput() == 0xAAAB);
    }

    // Test setPolarity16
    {
        PCF8575 expander;
        expander.begin();
        assert(expander.setPolarity16(0xFFFF));
    }

    printf("test_pcf8575: all assertions passed\n");
    return 0;
}
