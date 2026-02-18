#include "Arduino.h"
#include "esp32emu_gpio.h"
#include <cassert>
#include <cstdio>

static volatile int rising_count = 0;
static volatile int falling_count = 0;
static volatile int change_count = 0;

static void onRising() { rising_count++; }
static void onFalling() { falling_count++; }
static void onChange() { change_count++; }

int main() {
    auto& gpio = esp32emu::GPIOController::instance();
    gpio.reset();

    // Setup pins
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    digitalWrite(2, LOW);
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);

    // Attach interrupts
    attachInterrupt(2, onRising, RISING);
    attachInterrupt(3, onFalling, FALLING);
    attachInterrupt(4, onChange, CHANGE);

    // Test RISING: LOW->HIGH should fire
    rising_count = 0;
    digitalWrite(2, HIGH);
    assert(rising_count == 1);

    // HIGH->HIGH should NOT fire
    digitalWrite(2, HIGH);
    assert(rising_count == 1);

    // HIGH->LOW should NOT fire for RISING
    digitalWrite(2, LOW);
    assert(rising_count == 1);

    // LOW->HIGH again
    digitalWrite(2, HIGH);
    assert(rising_count == 2);

    // Test FALLING: HIGH->LOW should fire
    falling_count = 0;
    digitalWrite(3, LOW);
    assert(falling_count == 1);

    // LOW->HIGH should NOT fire for FALLING
    digitalWrite(3, HIGH);
    assert(falling_count == 1);

    // Test CHANGE: any change should fire
    change_count = 0;
    digitalWrite(4, HIGH);  // LOW->HIGH
    assert(change_count == 1);
    digitalWrite(4, LOW);   // HIGH->LOW
    assert(change_count == 2);
    digitalWrite(4, HIGH);
    assert(change_count == 3);

    // Test detachInterrupt
    detachInterrupt(2);
    rising_count = 0;
    digitalWrite(2, LOW);
    digitalWrite(2, HIGH);
    assert(rising_count == 0);  // detached, should not fire

    printf("test_interrupt: all assertions passed\n");
    return 0;
}
