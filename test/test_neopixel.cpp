// esp32emu test — Adafruit NeoPixel library
#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#include <cassert>
#include <cstdio>

int main() {
    Adafruit_NeoPixel strip(8, 6, NEO_GRB + NEO_KHZ800);
    strip.begin();

    // Initial state: all off
    assert(strip.numPixels() == 8);
    assert(strip.getPixelColor(0) == 0);

    // Set colors
    strip.setPixelColor(0, 255, 0, 0);   // Red
    strip.setPixelColor(1, 0, 255, 0);   // Green
    strip.setPixelColor(2, 0, 0, 255);   // Blue
    strip.setPixelColor(3, Adafruit_NeoPixel::Color(255, 255, 0)); // Yellow

    assert(strip.getPixelColor(0) == Adafruit_NeoPixel::Color(255, 0, 0));
    assert(strip.getPixelColor(1) == Adafruit_NeoPixel::Color(0, 255, 0));
    assert(strip.getPixelColor(2) == Adafruit_NeoPixel::Color(0, 0, 255));
    assert(strip.getPixelColor(3) == Adafruit_NeoPixel::Color(255, 255, 0));

    // Show (visual output)
    strip.show();

    // Brightness
    strip.setBrightness(128);
    assert(strip.getBrightness() == 128);
    strip.show();

    // Clear
    strip.clear();
    assert(strip.getPixelColor(0) == 0);
    assert(strip.getPixelColor(3) == 0);

    // Fill
    strip.fill(Adafruit_NeoPixel::Color(100, 50, 200));
    for (int i = 0; i < 8; i++)
        assert(strip.getPixelColor(i) == Adafruit_NeoPixel::Color(100, 50, 200));

    // ColorHSV
    uint32_t c = Adafruit_NeoPixel::ColorHSV(0, 255, 255); // Red
    assert(((c >> 16) & 0xFF) > 200); // R should be high

    // updateLength
    strip.updateLength(16);
    assert(strip.numPixels() == 16);

    strip.show();

    printf("test_neopixel: all assertions passed\n");
    return 0;
}
