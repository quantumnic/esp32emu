#include "TM1637Display.h"
#include <cassert>
#include <cstdio>

int main() {
    TM1637Display display(18, 19);
    assert(display.getClkPin() == 18);
    assert(display.getDioPin() == 19);

    // Brightness
    display.setBrightness(5, true);
    assert(display.getBrightness() == 5);
    assert(display.isOn());

    display.setBrightness(3, false);
    assert(display.getBrightness() == 3);
    assert(!display.isOn());

    // Show number
    display.showNumberDec(1234);
    assert(display.getLastNumber() == 1234);

    // Show with colon
    display.showNumberDecEx(1230, 0x40, false);
    assert(display.getLastNumber() == 1230);
    assert(display.isColonOn());

    display.showNumberDecEx(456, 0x00, true);
    assert(!display.isColonOn());
    assert(display.hasLeadingZero());

    // Segments
    uint8_t segs[] = {0x3F, 0x06, 0x5B, 0x4F};
    display.setSegments(segs);
    assert(display.getSegments()[0] == 0x3F);
    assert(display.getSegments()[3] == 0x4F);

    // Encode digit
    assert(TM1637Display::encodeDigit(0) == 0x3F);
    assert(TM1637Display::encodeDigit(1) == 0x06);
    assert(TM1637Display::encodeDigit(9) == 0x6F);

    // Clear
    display.clear();
    assert(display.getSegments()[0] == 0);

    printf("test_tm1637: all assertions passed\n");
    return 0;
}
