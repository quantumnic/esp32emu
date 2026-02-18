// Blink example for Raspberry Pi Pico (RP2040)
// Run with: esp32emu --board pico
#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    Serial.println("RP2040 Pico Blink Example");
    Serial.println("LED_BUILTIN = GP25");
    pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("LED ON");
    delay(500);

    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("LED OFF");
    delay(500);
}
