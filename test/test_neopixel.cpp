#include "Adafruit_NeoPixel.h"
#include <cassert>
#include <cstdio>

int main() {
    // Constructor
    Adafruit_NeoPixel strip(10, 5, NEO_GRB + NEO_KHZ800);
    strip.begin();

    // Num pixels
    assert(strip.numPixels() == 10);

    // All start black
    for (int i = 0; i < 10; i++) {
        assert(strip.getPixelColor(i) == 0);
    }

    // Set individual pixel (r,g,b)
    strip.setPixelColor(0, 255, 0, 0);
    assert(strip.getPixelColor(0) == Adafruit_NeoPixel::Color(255, 0, 0));

    // Set with packed color
    uint32_t green = Adafruit_NeoPixel::Color(0, 255, 0);
    strip.setPixelColor(1, green);
    assert(strip.getPixelColor(1) == green);

    // Out of bounds does nothing
    strip.setPixelColor(100, 255, 255, 255);
    assert(strip.getPixelColor(100) == 0);

    // Clear
    strip.clear();
    assert(strip.getPixelColor(0) == 0);
    assert(strip.getPixelColor(1) == 0);

    // Fill
    uint32_t blue = Adafruit_NeoPixel::Color(0, 0, 255);
    strip.fill(blue, 2, 3);
    assert(strip.getPixelColor(1) == 0);
    assert(strip.getPixelColor(2) == blue);
    assert(strip.getPixelColor(3) == blue);
    assert(strip.getPixelColor(4) == blue);
    assert(strip.getPixelColor(5) == 0);

    // Brightness
    strip.setBrightness(128);
    assert(strip.getBrightness() == 128);

    // Color helper
    assert(Adafruit_NeoPixel::Color(0xFF, 0x00, 0xFF) == 0xFF00FF);

    // ColorHSV (basic smoke test: red at hue 0)
    uint32_t hsv_red = Adafruit_NeoPixel::ColorHSV(0, 255, 255);
    uint8_t r = (hsv_red >> 16) & 0xFF;
    assert(r == 255); // Full red

    // Show (visual, just ensure it doesn't crash)
    strip.setPixelColor(0, 255, 128, 0);
    strip.show();

    // gamma32 passthrough
    assert(Adafruit_NeoPixel::gamma32(0x112233) == 0x112233);

    printf("test_neopixel: all assertions passed\n");
    return 0;
}
