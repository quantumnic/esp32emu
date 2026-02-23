// Waveshare ESP32-S3-Zero onboard RGB LED blink example
// Uses the built-in NeoPixel on the compact stamp module

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define RGB_PIN  21  // Onboard RGB LED on Waveshare ESP32-S3-Zero
#define NUM_LEDS 1

Adafruit_NeoPixel pixel(NUM_LEDS, RGB_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    Serial.begin(115200);
    Serial.println("Waveshare ESP32-S3-Zero RGB Blink");
    Serial.println("==================================");

    pixel.begin();
    pixel.setBrightness(50);
}

void loop() {
    // Cycle through colors
    const uint32_t colors[] = {
        pixel.Color(255, 0, 0),    // Red
        pixel.Color(0, 255, 0),    // Green
        pixel.Color(0, 0, 255),    // Blue
        pixel.Color(255, 255, 0),  // Yellow
        pixel.Color(255, 0, 255),  // Magenta
        pixel.Color(0, 255, 255),  // Cyan
    };
    const char* names[] = {"Red", "Green", "Blue", "Yellow", "Magenta", "Cyan"};

    for (int i = 0; i < 6; i++) {
        pixel.setPixelColor(0, colors[i]);
        pixel.show();
        Serial.print("🌈 RGB LED: ");
        Serial.println(names[i]);
        delay(500);
    }

    // Off
    pixel.setPixelColor(0, 0);
    pixel.show();
    Serial.println("💤 LED off");
    delay(500);
}
