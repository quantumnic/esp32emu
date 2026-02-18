#include "Arduino.h"
#include <cassert>
#include <cstdio>

int main() {
    // ledcSetup returns frequency
    double freq = ledcSetup(0, 5000, 8);
    assert(freq == 5000.0);

    // ledcAttachPin should not crash
    ledcAttachPin(2, 0);

    // ledcWrite + read
    ledcWrite(0, 128);

    // ledcReadFreq
    assert(ledcReadFreq(0) == 5000.0);

    // ledcDetachPin should not crash
    ledcDetachPin(2);

    // ledcWriteTone / ledcWriteNote should not crash
    ledcWriteTone(0, 440.0);
    ledcWriteNote(0, 0, 4);

    printf("test_ledc: all assertions passed\n");
    return 0;
}
