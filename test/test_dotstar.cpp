// test_dotstar: Adafruit DotStar (APA102) LED strip mock tests
#include "Adafruit_DotStar.h"
#include <cassert>
#include <cstdio>

int main() {
    Adafruit_DotStar strip(10, 4, 5, DOTSTAR_BGR);
    assert(!strip.isBegun());
    strip.begin();
    assert(strip.isBegun());
    assert(strip.numPixels() == 10);

    // All pixels start at 0
    for (uint16_t i = 0; i < 10; i++) {
        assert(strip.getPixelColor(i) == 0);
    }

    // Set pixel color (32-bit)
    strip.setPixelColor(0, 0xFF0000);
    assert(strip.getPixelColor(0) == 0xFF0000);

    // Set pixel color (r,g,b)
    strip.setPixelColor(1, 0, 255, 0);
    assert(strip.getPixelColor(1) == 0x00FF00);

    strip.setPixelColor(2, 0, 0, 255);
    assert(strip.getPixelColor(2) == 0x0000FF);

    // Out of range returns 0
    assert(strip.getPixelColor(100) == 0);

    // Brightness
    assert(strip.getBrightness() == 255);
    strip.setBrightness(128);
    assert(strip.getBrightness() == 128);

    // Show count
    assert(strip.getShowCount() == 0);
    strip.show();
    assert(strip.getShowCount() == 1);
    strip.show();
    assert(strip.getShowCount() == 2);

    // Clear
    strip.clear();
    for (uint16_t i = 0; i < 10; i++) {
        assert(strip.getPixelColor(i) == 0);
    }

    // Fill
    strip.fill(0x112233);
    for (uint16_t i = 0; i < 10; i++) {
        assert(strip.getPixelColor(i) == 0x112233);
    }

    // Partial fill
    strip.clear();
    strip.fill(0xAABBCC, 3, 4);
    assert(strip.getPixelColor(2) == 0);
    assert(strip.getPixelColor(3) == 0xAABBCC);
    assert(strip.getPixelColor(6) == 0xAABBCC);
    assert(strip.getPixelColor(7) == 0);

    // Color helper
    assert(Adafruit_DotStar::Color(255, 0, 0) == 0xFF0000);
    assert(Adafruit_DotStar::Color(0, 128, 64) == 0x008040);

    // ColorHSV — red should be near 0xFF0000
    uint32_t red = Adafruit_DotStar::ColorHSV(0, 255, 255);
    assert(((red >> 16) & 0xFF) == 255);

    // gamma32 passthrough
    assert(Adafruit_DotStar::gamma32(0x123456) == 0x123456);

    // HW SPI constructor
    Adafruit_DotStar hw_strip(5);
    hw_strip.begin();
    assert(hw_strip.numPixels() == 5);

    printf("test_dotstar: all assertions passed\n");
    return 0;
}
