// esp32emu — AMG88xx thermal camera test
#include "Arduino.h"
#include "Adafruit_AMG88xx.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_AMG88xx amg;
    assert(amg.begin());

    // Default thermistor
    assert(fabs(amg.readThermistor() - 25.0f) < 0.01f);

    // Inject thermistor
    amg._injectThermistor(30.5f);
    assert(fabs(amg.readThermistor() - 30.5f) < 0.01f);

    // Default pixels all zero
    float pixels[64];
    amg.readPixels(pixels);
    for (int i = 0; i < 64; i++) assert(fabs(pixels[i]) < 0.01f);

    // Inject single pixel
    amg._injectPixel(0, 36.5f);
    amg._injectPixel(63, 22.0f);
    amg.readPixels(pixels);
    assert(fabs(pixels[0] - 36.5f) < 0.01f);
    assert(fabs(pixels[63] - 22.0f) < 0.01f);
    assert(fabs(pixels[32]) < 0.01f);

    // Inject full array
    float data[64];
    for (int i = 0; i < 64; i++) data[i] = 20.0f + i * 0.5f;
    amg._injectPixels(data);
    amg.readPixels(pixels);
    assert(fabs(pixels[0] - 20.0f) < 0.01f);
    assert(fabs(pixels[63] - 51.5f) < 0.01f);

    // Frame rate
    assert(amg.getFrameRate() == AMG88xx_FPS_10);
    amg.setFrameRate(AMG88xx_FPS_1);
    assert(amg.getFrameRate() == AMG88xx_FPS_1);

    // Power mode
    assert(amg.getPowerMode() == AMG88xx_NORMAL_MODE);
    amg.setPowerMode(AMG88xx_SLEEP_MODE);
    assert(amg.getPowerMode() == AMG88xx_SLEEP_MODE);

    // Moving average
    assert(!amg.getMovingAverageMode());
    amg.setMovingAverageMode(true);
    assert(amg.getMovingAverageMode());

    // Interrupts
    assert(!amg.isInterruptEnabled());
    amg.enableInterrupt();
    assert(amg.isInterruptEnabled());
    amg.setInterruptLevels(35.0f, 15.0f, 2.0f);
    assert(fabs(amg.getInterruptHighLevel() - 35.0f) < 0.01f);
    assert(fabs(amg.getInterruptLowLevel() - 15.0f) < 0.01f);
    amg.disableInterrupt();
    assert(!amg.isInterruptEnabled());

    printf("test_amg88xx: all assertions passed\n");
    return 0;
}
