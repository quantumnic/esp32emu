// test_ili9341: Adafruit ILI9341 TFT display mock tests
#include "Adafruit_ILI9341.h"
#include <cassert>
#include <cstdio>

int main() {
    Adafruit_ILI9341 tft(5, 4, 16);

    // Not initialized yet
    assert(!tft.isInitialized());

    tft.begin();
    assert(tft.isInitialized());

    // Default dimensions
    assert(tft.width() == 240);
    assert(tft.height() == 320);

    // Rotation
    tft.setRotation(1);
    assert(tft.getRotation() == 1);
    assert(tft.width() == 320);
    assert(tft.height() == 240);
    tft.setRotation(0);

    // Fill screen
    tft.fillScreen(ILI9341_RED);
    assert(tft.readPixel(0, 0) == ILI9341_RED);
    assert(tft.readPixel(100, 100) == ILI9341_RED);

    // Draw pixel
    tft.drawPixel(10, 20, ILI9341_BLUE);
    assert(tft.readPixel(10, 20) == ILI9341_BLUE);
    assert(tft.readPixel(11, 20) == ILI9341_RED); // unchanged

    // Out of bounds
    tft.drawPixel(-1, -1, ILI9341_GREEN);
    tft.drawPixel(240, 320, ILI9341_GREEN);
    assert(tft.readPixel(-1, -1) == 0); // returns 0

    // Fill rect
    tft.fillScreen(ILI9341_BLACK);
    tft.fillRect(10, 10, 5, 5, ILI9341_WHITE);
    assert(tft.readPixel(10, 10) == ILI9341_WHITE);
    assert(tft.readPixel(14, 14) == ILI9341_WHITE);
    assert(tft.readPixel(15, 15) == ILI9341_BLACK);

    // Text
    tft.setCursor(0, 0);
    tft.setTextColor(ILI9341_YELLOW);
    tft.setTextSize(2);
    assert(tft.getTextColor() == ILI9341_YELLOW);
    assert(tft.getTextSize() == 2);
    tft.print("Test");
    assert(strcmp(tft.getLastText(), "Test") == 0);

    // Cursor advances
    assert(tft.getCursorX() == 4 * 6 * 2); // 4 chars * 6px * size2

    // Invert
    assert(!tft.isInverted());
    tft.invertDisplay(true);
    assert(tft.isInverted());

    // Draw line
    tft.fillScreen(ILI9341_BLACK);
    tft.drawLine(0, 0, 10, 0, ILI9341_GREEN);
    for (int x = 0; x <= 10; x++) {
        assert(tft.readPixel(x, 0) == ILI9341_GREEN);
    }

    // Circle - just check center area has some pixels
    tft.fillScreen(ILI9341_BLACK);
    tft.fillCircle(50, 50, 5, ILI9341_CYAN);
    assert(tft.readPixel(50, 50) == ILI9341_CYAN);
    assert(tft.readPixel(50, 48) == ILI9341_CYAN);

    printf("test_ili9341: all assertions passed\n");
    return 0;
}
