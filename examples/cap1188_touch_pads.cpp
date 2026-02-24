// esp32emu example — Adafruit CAP1188 Capacitive Touch Sensor
#include <Arduino.h>
#include <Adafruit_CAP1188.h>

Adafruit_CAP1188 cap;

void setup() {
    Serial.begin(115200);
    Serial.println("=== CAP1188 Capacitive Touch Demo ===");

    if (!cap.begin()) {
        Serial.println("❌ CAP1188 not found!");
        return;
    }
    Serial.printf("✅ CAP1188 found — Product ID: 0x%02X, Manufacturer: 0x%02X\n",
                  cap.readProductID(), cap.readManufacturerID());

    cap.setSensitivity(32);
    cap.setMultiTouch(true);

    // Simulate some touches for demo
    cap.test_setTouched(0b00000101); // pads 0 and 2
    cap.test_setDelta(0, 30);
    cap.test_setDelta(2, 45);
}

void loop() {
    uint8_t touched = cap.touched();
    if (touched) {
        Serial.print("Touched pads: ");
        for (uint8_t i = 0; i < 8; i++) {
            if (touched & (1 << i)) {
                Serial.printf("[%d Δ=%d] ", i, cap.readDelta(i));
            }
        }
        Serial.println();
    }
    delay(100);
}
