// test_ws2812fx: WS2812FX LED effects library mock tests
#include "WS2812FX.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    WS2812FX strip(30, 5);
    strip.init();

    // Mode management
    assert(strip.getMode() == FX_MODE_STATIC);
    strip.setMode(FX_MODE_RAINBOW_CYCLE);
    assert(strip.getMode() == FX_MODE_RAINBOW_CYCLE);
    assert(strcmp(strip.getModeName(FX_MODE_RAINBOW_CYCLE), "Rainbow Cycle") == 0);
    assert(strip.getModeCount() == MODE_COUNT);

    // Out of range mode wraps to 0
    strip.setMode(255);
    assert(strip.getMode() == 0);

    // Speed
    strip.setSpeed(500);
    assert(strip.getSpeed() == 500);

    // Color
    strip.setColor(0xFF0000);
    assert(strip.getColor() == 0xFF0000);
    strip.setColor(0, 255, 0);
    assert(strip.getColor() == 0x00FF00);
    strip.setColors(1, 0x0000FF);
    assert(strip.getColor(1) == 0x0000FF);

    // Brightness
    strip.setBrightness(128);
    assert(strip.getBrightness() == 128);

    // Reverse
    assert(!strip.getReverse());
    strip.setReverse(true);
    assert(strip.getReverse());

    // Start/stop and service
    assert(!strip.isRunning());
    strip.start();
    assert(strip.isRunning());
    assert(strip.getServiceCount() == 0);
    strip.service();
    assert(strip.getServiceCount() == 1);
    strip.service();
    assert(strip.getServiceCount() == 2);
    strip.stop();
    assert(!strip.isRunning());
    strip.service(); // should not count when stopped
    assert(strip.getServiceCount() == 2);

    // Custom mode callback
    int custom_called = 0;
    strip.setCustomMode([&]() { custom_called++; });
    strip.start();
    strip.service();
    assert(custom_called == 1);

    // Reset service count
    strip.resetServiceCount();
    assert(strip.getServiceCount() == 0);

    // Mode names for all modes
    for (uint8_t i = 0; i < MODE_COUNT; i++) {
        assert(strip.getModeName(i) != nullptr);
        assert(strlen(strip.getModeName(i)) > 0);
    }
    assert(strcmp(strip.getModeName(255), "Unknown") == 0);

    printf("test_ws2812fx: all assertions passed\n");
    return 0;
}
