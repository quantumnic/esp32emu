// esp32emu — test_adafruit_tca9548a: TCA9548A I2C multiplexer mock test
#include "Adafruit_TCA9548A.h"
#include <cassert>
#include <cstdio>

int main() {
    Adafruit_TCA9548A mux;

    assert(!mux.test_isBegun());
    assert(mux.begin());
    assert(mux.test_isBegun());
    assert(mux.test_getAddr() == 0x70);

    // No channel selected initially
    assert(mux.getChannel() == 0xFF);

    // Select channels 0-7
    for (uint8_t ch = 0; ch < 8; ch++) {
        mux.selectChannel(ch);
        assert(mux.getChannel() == ch);
        assert(mux.test_getChannel() == ch);
    }

    // Invalid channel (>7) should be ignored
    mux.selectChannel(3);
    mux.selectChannel(8);
    assert(mux.getChannel() == 3);

    // Disable all
    mux.disableAll();
    assert(mux.getChannel() == 0xFF);

    // Custom address
    Adafruit_TCA9548A mux2(0x71);
    mux2.begin();
    assert(mux2.test_getAddr() == 0x71);

    printf("test_adafruit_tca9548a: all assertions passed\n");
    return 0;
}
