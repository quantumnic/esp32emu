#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// M5Atom Lite has a single SK6812 LED on GPIO 27
#define LED_PIN   27
#define NUM_LEDS  1

Adafruit_NeoPixel pixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
    Serial.begin(115200);
    Serial.println("M5Atom Lite — NeoPixel Blink");

    pixel.begin();
    pixel.setBrightness(50);
    pixel.show();
}

void loop() {
    // Cycle through red, green, blue
    static const uint32_t colors[] = {
        Adafruit_NeoPixel::Color(255, 0, 0),
        Adafruit_NeoPixel::Color(0, 255, 0),
        Adafruit_NeoPixel::Color(0, 0, 255),
    };
    static int idx = 0;

    pixel.setPixelColor(0, colors[idx]);
    pixel.show();

    Serial.print("Color index: ");
    Serial.println(idx);

    idx = (idx + 1) % 3;
    delay(500);
}
