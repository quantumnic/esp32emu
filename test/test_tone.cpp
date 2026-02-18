// esp32emu test — tone/noTone/pulseIn/shiftOut
#include "Arduino.h"
#include <cassert>
#include <cstdio>

int main() {
    // tone/noTone should not crash
    tone(8, 440, 500);
    tone(8, 1000);
    noTone(8);

    // pulseIn returns simulated value
    unsigned long us = pulseIn(7, HIGH);
    assert(us > 0);

    // shiftOut/shiftIn should not crash
    shiftOut(11, 13, MSBFIRST, 0xAB);
    uint8_t val = shiftIn(12, 13, LSBFIRST);
    (void)val;

    printf("test_tone: all assertions passed\n");
    return 0;
}
