#include "Adafruit_AS7341.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_AS7341 as;
    assert(as.begin());

    // Default channel values are 0
    assert(as.readAllChannels());
    assert(as.getChannel(AS7341_CHANNEL_415nm_F1) == 0);

    // Set individual channels
    as.esp32emu_set_channel(AS7341_CHANNEL_415nm_F1, 120);
    as.esp32emu_set_channel(AS7341_CHANNEL_480nm_F3, 450);
    as.esp32emu_set_channel(AS7341_CHANNEL_630nm_F7, 800);
    as.esp32emu_set_channel(AS7341_CHANNEL_NIR, 200);
    assert(as.getChannel(AS7341_CHANNEL_415nm_F1) == 120);
    assert(as.getChannel(AS7341_CHANNEL_480nm_F3) == 450);
    assert(as.getChannel(AS7341_CHANNEL_630nm_F7) == 800);
    assert(as.getChannel(AS7341_CHANNEL_NIR) == 200);

    // Read all channels to buffer
    uint16_t buf[10];
    assert(as.readAllChannels(buf));
    assert(buf[0] == 120);
    assert(buf[2] == 450);
    assert(buf[6] == 800);
    assert(buf[9] == 200);

    // Set all channels at once
    uint16_t vals[10] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    as.esp32emu_set_all_channels(vals);
    assert(as.getChannel(AS7341_CHANNEL_555nm_F5) == 500);
    assert(as.getChannel(AS7341_CHANNEL_CLEAR) == 900);

    // Gain settings
    as.setGain(AS7341_GAIN_256X);
    assert(as.getGain() == AS7341_GAIN_256X);

    // ATIME / ASTEP
    as.setATIME(29);
    as.setASTEP(599);
    assert(as.getATIME() == 29);
    assert(as.getASTEP() == 599);

    // Integration time
    float tint = as.getTINT();
    assert(tint > 0.0f);

    // Flicker detection
    as.esp32emu_set_flicker(50);
    assert(as.detectFlickerHz() == 50);

    // LED control
    as.enableLED(true);
    as.setLEDCurrent(AS7341_LED_100MA);

    printf("test_as7341: all assertions passed\n");
    return 0;
}
