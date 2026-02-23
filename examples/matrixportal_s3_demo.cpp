// MatrixPortal S3 LED Matrix Demo
// Board: matrixportal-s3
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>

#define MATRIX_PIN    7
#define NUM_LEDS      256  // 32x8 matrix
#define BUTTON_UP     2
#define BUTTON_DOWN   3

Adafruit_NeoPixel matrix(NUM_LEDS, MATRIX_PIN, NEO_GRB + NEO_KHZ800);

uint8_t brightness = 50;
uint8_t hueOffset = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("MatrixPortal S3 LED Matrix Demo");

    pinMode(BUTTON_UP, INPUT_PULLUP);
    pinMode(BUTTON_DOWN, INPUT_PULLUP);

    matrix.begin();
    matrix.setBrightness(brightness);
    matrix.show();

    WiFi.begin("MyNetwork", "password");
    Serial.println("Connecting to WiFi...");
}

void rainbowWave() {
    for (int i = 0; i < NUM_LEDS; i++) {
        int row = i / 32;
        int col = i % 32;
        uint8_t hue = (col * 8 + row * 32 + hueOffset) & 0xFF;
        // Simple HSV to RGB
        uint8_t r = 0, g = 0, b = 0;
        uint8_t region = hue / 43;
        uint8_t remainder = (hue - region * 43) * 6;
        switch (region) {
            case 0: r=255; g=remainder; break;
            case 1: r=255-remainder; g=255; break;
            case 2: g=255; b=remainder; break;
            case 3: g=255-remainder; b=255; break;
            case 4: b=255; r=remainder; break;
            default: r=255; b=255-remainder; break;
        }
        matrix.setPixelColor(i, matrix.Color(r, g, b));
    }
    matrix.show();
    hueOffset += 2;
}

void loop() {
    if (digitalRead(BUTTON_UP) == LOW) {
        brightness = min(255, brightness + 10);
        matrix.setBrightness(brightness);
        Serial.print("Brightness: ");
        Serial.println(brightness);
        delay(200);
    }
    if (digitalRead(BUTTON_DOWN) == LOW) {
        brightness = max(5, brightness - 10);
        matrix.setBrightness(brightness);
        Serial.print("Brightness: ");
        Serial.println(brightness);
        delay(200);
    }

    rainbowWave();
    delay(50);
}
