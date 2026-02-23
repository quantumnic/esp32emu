// DotStar (APA102) rainbow cycle demo
#include <Adafruit_DotStar.h>

#define NUM_LEDS 30
#define DATA_PIN 4
#define CLOCK_PIN 5

Adafruit_DotStar strip(NUM_LEDS, DATA_PIN, CLOCK_PIN, DOTSTAR_BRG);

uint16_t hueOffset = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("DotStar Rainbow Demo");

    strip.begin();
    strip.setBrightness(80);
    strip.show();
}

void loop() {
    for (uint16_t i = 0; i < strip.numPixels(); i++) {
        uint16_t hue = hueOffset + (i * 65536 / strip.numPixels());
        strip.setPixelColor(i, Adafruit_DotStar::ColorHSV(hue));
    }
    strip.show();

    hueOffset += 256;
    delay(20);
}
