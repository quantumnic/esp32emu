// XIAO ESP32-C3 blink demo — uses onboard LED and WiFi scan
#include <Arduino.h>
#include <WiFi.h>

// XIAO ESP32-C3 onboard LED is on GPIO 10
#define LED_PIN 10

void setup() {
    Serial.begin(115200);
    Serial.println("XIAO ESP32-C3 Demo");

    pinMode(LED_PIN, OUTPUT);

    // Quick WiFi scan
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("Scanning WiFi...");
    int n = WiFi.scanNetworks();
    Serial.printf("Found %d networks\n", n);
    for (int i = 0; i < n; i++) {
        Serial.printf("  %d: %s (%d dBm)\n", i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i));
    }
}

void loop() {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ON");
    delay(500);

    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF");
    delay(500);
}
