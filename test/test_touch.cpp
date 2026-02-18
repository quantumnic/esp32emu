#include "Arduino.h"
#include "esp32emu_touch.h"
#include <cassert>
#include <cstdio>

static volatile int touch_count = 0;
static void onTouch() { touch_count++; }

int main() {
    auto& tc = esp32emu::TouchController::instance();
    tc.reset();

    // Default untouched value should be high
    uint16_t val = touchRead(4);
    assert(val == 60);  // default untouched

    // Set a specific touch value
    tc.setTouchValue(4, 500);
    assert(touchRead(4) == 500);

    // Attach interrupt with threshold
    touchAttachInterrupt(4, onTouch, 30);

    // Simulate touch (value drops below threshold)
    touch_count = 0;
    tc.simulateTouch(4, 10);  // 10 < 30 threshold
    assert(touch_count == 1);
    assert(touchRead(4) == 10);

    // Release touch
    tc.releaseTouch(4);
    assert(touchRead(4) == 60);

    // Touch again
    tc.simulateTouch(4, 5);
    assert(touch_count == 2);

    // Setting value above threshold should not fire
    tc.releaseTouch(4);
    tc.setTouchValue(4, 50);  // 50 > 30, no fire
    assert(touch_count == 2);

    // Detach interrupt
    touchDetachInterrupt(4);
    tc.releaseTouch(4);
    tc.simulateTouch(4, 10);
    assert(touch_count == 2);  // should not fire after detach

    // Test getThreshold
    touchAttachInterrupt(2, onTouch, 40);
    assert(tc.getThreshold(2) == 40);

    printf("test_touch: all assertions passed\n");
    return 0;
}
