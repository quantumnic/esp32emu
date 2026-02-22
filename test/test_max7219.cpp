#include "MAX7219.h"
#include <cassert>
#include <cstdio>

int main() {
    MAX7219 matrix(23, 5, 18, 2);
    assert(!matrix.isBegun());
    assert(matrix.getNumDevices() == 2);

    matrix.begin();
    assert(matrix.isBegun());

    // Set intensity
    matrix.setIntensity(0, 10);
    assert(matrix.getIntensity() == 10);

    // Set/get individual LEDs
    matrix.setLed(0, 3, 5, true);
    assert(matrix.getLed(0, 3, 5));
    assert(!matrix.getLed(0, 3, 4));

    matrix.setLed(0, 3, 5, false);
    assert(!matrix.getLed(0, 3, 5));

    // Set row
    matrix.setRow(0, 0, 0xFF);
    assert(matrix.getRow(0, 0) == 0xFF);

    // Set column
    matrix.setColumn(1, 2, 0x81);  // top and bottom bits
    assert(matrix.getLed(1, 0, 2));
    assert(!matrix.getLed(1, 1, 2));
    assert(matrix.getLed(1, 7, 2));

    // Clear
    matrix.clearDisplay(0);
    assert(matrix.getRow(0, 0) == 0);

    // Set digit
    matrix.setDigit(0, 0, 5, false);
    assert(matrix.getRow(0, 0) != 0);  // digit 5 has segments

    // Out of bounds safe
    matrix.setLed(5, 0, 0, true);  // device 5 doesn't exist
    matrix.setRow(0, 10, 0xFF);     // row 10 doesn't exist

    printf("test_max7219: all assertions passed\n");
    return 0;
}
