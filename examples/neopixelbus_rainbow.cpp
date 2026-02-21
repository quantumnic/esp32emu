// NeoPixelBus Rainbow example — animated rainbow on LED strip
#include "Arduino.h"
#include "NeoPixelBus.h"

const uint16_t NUM_LEDS = 16;
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(NUM_LEDS, 2);

uint16_t offset = 0;

void setup() {
    Serial.begin(115200);
    strip.Begin();
    strip.Show();
    Serial.println("NeoPixelBus Rainbow starting...");
}

void loop() {
    for (uint16_t i = 0; i < NUM_LEDS; i++) {
        float hue = (float)((i * 360 / NUM_LEDS + offset) % 360) / 360.0f;
        // Simple HSV to RGB
        int h = (int)(hue * 6.0f);
        float f = hue * 6.0f - h;
        uint8_t v = 255;
        uint8_t p = 0;
        uint8_t q = (uint8_t)(255 * (1.0f - f));
        uint8_t t = (uint8_t)(255 * f);
        RgbColor c;
        switch (h % 6) {
            case 0: c = RgbColor(v, t, p); break;
            case 1: c = RgbColor(q, v, p); break;
            case 2: c = RgbColor(p, v, t); break;
            case 3: c = RgbColor(p, q, v); break;
            case 4: c = RgbColor(t, p, v); break;
            case 5: c = RgbColor(v, p, q); break;
        }
        strip.SetPixelColor(i, c);
    }
    strip.Show();
    offset = (offset + 10) % 360;
    delay(50);
}
