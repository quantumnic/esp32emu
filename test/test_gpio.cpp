#include "Arduino.h"
#include <cassert>
#include <cstdio>

static int isr_count = 0;
static void test_isr() { isr_count++; }

int main() {
    auto& gpio = esp32emu::GPIOController::instance();
    gpio.reset();

    // Test pinMode and digitalWrite/digitalRead
    pinMode(13, OUTPUT);
    assert(gpio.getPin(13).mode == OUTPUT);

    digitalWrite(13, HIGH);
    assert(digitalRead(13) == HIGH);

    digitalWrite(13, LOW);
    assert(digitalRead(13) == LOW);

    // Test analogRead/analogWrite
    gpio.setAnalogInput(A0, 2048);
    assert(analogRead(A0) == 2048);

    analogWrite(5, 128);
    assert(gpio.getPin(5).analog == 128);
    assert(digitalRead(5) == HIGH);

    analogWrite(5, 0);
    assert(digitalRead(5) == LOW);

    // Test interrupt
    isr_count = 0;
    pinMode(4, INPUT);
    attachInterrupt(4, test_isr, RISING);

    digitalWrite(4, HIGH); // should trigger RISING
    assert(isr_count == 1);

    digitalWrite(4, LOW);  // should not trigger RISING
    assert(isr_count == 1);

    digitalWrite(4, HIGH); // trigger again
    assert(isr_count == 2);

    detachInterrupt(4);
    digitalWrite(4, LOW);
    digitalWrite(4, HIGH); // no ISR now
    assert(isr_count == 2);

    printf("test_gpio: all assertions passed\n");
    return 0;
}
