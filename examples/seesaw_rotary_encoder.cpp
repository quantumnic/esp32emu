#include <Arduino.h>
#include <Adafruit_seesaw.h>

Adafruit_seesaw_RotaryEncoder encoder;
int32_t lastPosition = 0;

void setup() {
    Serial.begin(115200);
    if (!encoder.begin(0x36)) {
        Serial.println("Seesaw rotary encoder not found!");
        return;
    }
    Serial.print("Seesaw version: 0x");
    Serial.println(encoder.getVersion(), HEX);
    encoder.setEncoderPosition(0);
    encoder.enableEncoderInterrupt();

    // Set NeoPixel to green
    encoder.setBrightness(50);
    encoder.setPixelColor(0, 0x00FF00);
    encoder.show();

    Serial.println("Rotary encoder ready. Turn to adjust.");
}

void loop() {
    int32_t position = encoder.getEncoderPosition();
    if (position != lastPosition) {
        Serial.print("Position: ");
        Serial.println(position);

        // Color based on position
        uint32_t color;
        if (position > 0) color = 0x00FF00;       // green
        else if (position < 0) color = 0xFF0000;   // red
        else color = 0x0000FF;                      // blue

        encoder.setPixelColor(0, color);
        encoder.show();
        lastPosition = position;
    }
    delay(10);
}
