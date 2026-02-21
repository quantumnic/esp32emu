// esp32emu example: MPR121 Capacitive Touch Pads
// Reads 12 touch channels and displays which pads are touched.

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPR121.h>

Adafruit_MPR121 cap;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!cap.begin(0x5A)) {
        Serial.println("MPR121 not found!");
        while (1) delay(10);
    }
    cap.setThresholds(12, 6);

    // Test: simulate pads 0, 3, 7 touched
    cap.setChannelTouched(0, true);
    cap.setChannelTouched(3, true);
    cap.setChannelTouched(7, true);

    Serial.println("MPR121 Touch Pad Demo");
}

void loop() {
    uint16_t state = cap.touched();
    Serial.print("Touched: ");
    for (int i = 0; i < 12; i++) {
        if (state & (1 << i)) {
            Serial.print(i); Serial.print(" ");
        }
    }
    Serial.println();
    delay(200);
}
