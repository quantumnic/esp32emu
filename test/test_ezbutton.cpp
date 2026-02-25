// esp32emu — test_ezbutton: ezButton debounce library mock test
#include "ezButton.h"
#include <cassert>
#include <cstdio>

int main() {
    ezButton btn(4);

    // Initial state: HIGH (not pressed, active-low)
    assert(btn.getState() == HIGH);
    assert(!btn.isPressed());
    assert(!btn.isReleased());
    assert(btn.esp32emu_get_pin() == 4);

    // Simulate press (HIGH -> LOW transition)
    btn.esp32emu_press();
    btn.loop();
    assert(btn.isPressed());
    assert(!btn.isReleased());
    assert(btn.getState() == LOW);

    // Second loop: no transition
    btn.loop();
    assert(!btn.isPressed());
    assert(!btn.isReleased());

    // Simulate release (LOW -> HIGH)
    btn.esp32emu_release();
    btn.loop();
    assert(!btn.isPressed());
    assert(btn.isReleased());

    // Count mode: FALLING (default)
    ezButton btn2(5);
    btn2.setCountMode(ezButton::COUNT_FALLING);
    assert(btn2.getCount() == 0);

    btn2.esp32emu_press();
    btn2.loop();
    assert(btn2.getCount() == 1);
    btn2.esp32emu_release();
    btn2.loop();
    assert(btn2.getCount() == 1); // only falling edges

    // Count mode: BOTH
    ezButton btn3(6);
    btn3.setCountMode(ezButton::COUNT_BOTH);
    btn3.esp32emu_press();
    btn3.loop();
    btn3.esp32emu_release();
    btn3.loop();
    assert(btn3.getCount() == 2);

    // Reset count
    btn3.resetCount();
    assert(btn3.getCount() == 0);

    // Debounce time setter
    btn.setDebounceTime(100);

    // Raw state
    btn.esp32emu_set_state(LOW);
    assert(btn.getStateRaw() == LOW);

    printf("test_ezbutton: all assertions passed\n");
    return 0;
}
