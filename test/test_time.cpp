#include "Arduino.h"
#include <cassert>
#include <cstdio>

int main() {
    unsigned long t1 = millis();
    delay(50);
    unsigned long t2 = millis();
    unsigned long diff = t2 - t1;

    // Should be roughly 50ms (allow 30-200ms for CI variance)
    assert(diff >= 30 && diff <= 200);

    unsigned long u1 = micros();
    delayMicroseconds(1000);
    unsigned long u2 = micros();
    assert(u2 > u1);

    printf("test_time: all assertions passed (delay measured: %lums)\n", diff);
    return 0;
}
