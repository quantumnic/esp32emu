// esp32emu example — ESP32-S3-BOX-3 demo with touch display
// Board: esp32-s3-box-3

#include <Arduino.h>
#include <WiFi.h>

#define LED_PIN 47
#define BUTTON_PIN 0

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    Serial.println("ESP32-S3-BOX-3 Demo");
    Serial.println("2.4\" Touch LCD + Dual Mic + Speaker");

    WiFi.begin("TestNetwork", "password123");
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("\nConnected! IP: ");
        Serial.println(WiFi.localIP());
    }
}

void loop() {
    // Blink RGB LED
    digitalWrite(LED_PIN, HIGH);
    Serial.println("💡 RGB LED ON");
    delay(500);
    digitalWrite(LED_PIN, LOW);
    Serial.println("💡 RGB LED OFF");
    delay(500);

    if (digitalRead(BUTTON_PIN) == LOW) {
        Serial.println("🔘 BOOT button pressed");
    }
}
