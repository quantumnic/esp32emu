// NeoPixel RGB Rainbow — ANSI true-color terminal output
// esp32emu run examples/neopixel_rainbow/neopixel_rainbow.ino
#include "Arduino.h"
#include <cmath>

#define NUM_LEDS 16
#define LED_PIN 6

struct RGB { uint8_t r, g, b; };

RGB leds[NUM_LEDS];
int offset = 0;

// HSV to RGB conversion
RGB hsvToRgb(float h, float s, float v) {
    float c = v * s;
    float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
    float m = v - c;
    float r, g, b;
    if (h < 60)       { r=c; g=x; b=0; }
    else if (h < 120) { r=x; g=c; b=0; }
    else if (h < 180) { r=0; g=c; b=x; }
    else if (h < 240) { r=0; g=x; b=c; }
    else if (h < 300) { r=x; g=0; b=c; }
    else               { r=c; g=0; b=x; }
    return {(uint8_t)((r+m)*255), (uint8_t)((g+m)*255), (uint8_t)((b+m)*255)};
}

void showStrip() {
    Serial.print("\r  ");
    for (int i = 0; i < NUM_LEDS; i++) {
        Serial.printf("\033[48;2;%d;%d;%dm  \033[0m", leds[i].r, leds[i].g, leds[i].b);
    }
    Serial.print("  ");
    Serial.flush();
}

void setup() {
    Serial.begin(115200);
    Serial.println("🌈 NeoPixel Rainbow — " + String(NUM_LEDS) + " LEDs on pin " + String(LED_PIN));
    Serial.println("Using ANSI true-color (24-bit) terminal rendering\n");
}

void loop() {
    // Rainbow cycle
    for (int i = 0; i < NUM_LEDS; i++) {
        float hue = fmod((i * 360.0 / NUM_LEDS) + offset, 360.0);
        leds[i] = hsvToRgb(hue, 1.0, 1.0);
    }
    showStrip();
    offset = (offset + 5) % 360;
    delay(50);
}
