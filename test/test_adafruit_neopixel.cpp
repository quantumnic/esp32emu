#include "Arduino.h"
#include "Adafruit_NeoPixel.h"
#include <cassert>
#include <cstdio>

int main() {
    // Test 1: Constructor and begin
    {
        Adafruit_NeoPixel strip(10, 5, NEO_GRB + NEO_KHZ800);
        strip.begin();
        assert(strip.numPixels() == 10);
        assert(strip.getPin() == 5);
    }

    // Test 2: Set and get pixel color
    {
        Adafruit_NeoPixel strip(5, 2);
        strip.begin();
        strip.setPixelColor(0, 255, 0, 0);
        strip.setPixelColor(1, 0, 255, 0);
        strip.setPixelColor(2, 0, 0, 255);
        assert(strip.getPixelColor(0) == 0xFF0000);
        assert(strip.getPixelColor(1) == 0x00FF00);
        assert(strip.getPixelColor(2) == 0x0000FF);
    }

    // Test 3: Set pixel with packed color
    {
        Adafruit_NeoPixel strip(3, 4);
        strip.begin();
        strip.setPixelColor(0, Adafruit_NeoPixel::Color(128, 64, 32));
        assert(strip.getPixelColor(0) == 0x804020);
    }

    // Test 4: Brightness
    {
        Adafruit_NeoPixel strip(5, 2);
        strip.begin();
        assert(strip.getBrightness() == 255);
        strip.setBrightness(128);
        assert(strip.getBrightness() == 128);
    }

    // Test 5: Clear
    {
        Adafruit_NeoPixel strip(3, 2);
        strip.begin();
        strip.setPixelColor(0, 255, 255, 255);
        strip.setPixelColor(1, 255, 255, 255);
        strip.clear();
        assert(strip.getPixelColor(0) == 0);
        assert(strip.getPixelColor(1) == 0);
    }

    // Test 6: Fill
    {
        Adafruit_NeoPixel strip(5, 2);
        strip.begin();
        strip.fill(0xFF0000, 1, 3);
        assert(strip.getPixelColor(0) == 0);
        assert(strip.getPixelColor(1) == 0xFF0000);
        assert(strip.getPixelColor(2) == 0xFF0000);
        assert(strip.getPixelColor(3) == 0xFF0000);
        assert(strip.getPixelColor(4) == 0);
    }

    // Test 7: Out of bounds ignored
    {
        Adafruit_NeoPixel strip(3, 2);
        strip.begin();
        strip.setPixelColor(10, 255, 0, 0); // should not crash
        assert(strip.getPixelColor(10) == 0); // out of range returns 0
    }

    // Test 8: Color static
    {
        assert(Adafruit_NeoPixel::Color(0, 0, 0) == 0);
        assert(Adafruit_NeoPixel::Color(255, 255, 255) == 0xFFFFFF);
        assert(Adafruit_NeoPixel::Color(0x12, 0x34, 0x56) == 0x123456);
    }

    // Test 9: ColorHSV
    {
        uint32_t red = Adafruit_NeoPixel::ColorHSV(0, 255, 255);
        assert(((red >> 16) & 0xFF) > 200); // should be mostly red
    }

    // Test 10: updateLength
    {
        Adafruit_NeoPixel strip(3, 2);
        strip.begin();
        assert(strip.numPixels() == 3);
        strip.updateLength(10);
        assert(strip.numPixels() == 10);
        strip.setPixelColor(9, 0, 255, 0);
        assert(strip.getPixelColor(9) == 0x00FF00);
    }

    // Test 11: Show (should not crash)
    {
        Adafruit_NeoPixel strip(3, 2);
        strip.begin();
        strip.setPixelColor(0, 255, 0, 0);
        strip.show(); // visual output to stderr
    }

    // Test 12: gamma32 passthrough
    {
        assert(Adafruit_NeoPixel::gamma32(0x123456) == 0x123456);
    }

    printf("test_adafruit_neopixel: all assertions passed\n");
    return 0;
}
