#include "Servo.h"
#include <cassert>
#include <cstdio>

int main() {
    Servo s;

    // Not attached initially
    assert(!s.attached());

    // Attach
    s.attach(9);
    assert(s.attached());

    // Write and read angle
    s.write(90);
    assert(s.read() == 90);

    s.write(0);
    assert(s.read() == 0);

    s.write(180);
    assert(s.read() == 180);

    // Clamping
    s.write(-10);
    assert(s.read() == 0);

    s.write(200);
    assert(s.read() == 180);

    // Microseconds
    s.writeMicroseconds(1472); // ~90 degrees with default 544-2400 range
    int angle = s.read();
    assert(angle >= 85 && angle <= 95);

    // readMicroseconds
    s.write(0);
    int us = s.readMicroseconds();
    assert(us >= 540 && us <= 550);

    // Detach
    s.detach();
    assert(!s.attached());

    // Write when detached should be no-op
    s.write(45);
    // angle shouldn't change (still 0 from before detach)

    printf("test_servo: all assertions passed\n");
    return 0;
}
