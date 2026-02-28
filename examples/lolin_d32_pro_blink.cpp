// esp32emu example — LOLIN D32 Pro board blink
// Board: LOLIN D32 Pro (ESP32-WROVER, 16MB Flash, 4MB PSRAM)
#include <Arduino.h>

#define LED_PIN 2  // Built-in LED on LOLIN D32 Pro

void setup() {
    Serial.begin(115200);
    Serial.println("LOLIN D32 Pro — Blink Example");
    Serial.println("Features: 16MB Flash, 4MB PSRAM, TFT connector, SD slot, LiPo charger");
    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ON");
    delay(500);

    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF");
    delay(500);
}
