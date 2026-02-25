// esp32emu example: Heltec WiFi LoRa 32 V3 board demo
// Shows board info, OLED display, and LoRa TX/RX
#include "Arduino.h"
#include "SSD1306Wire.h"
#include "WiFi.h"

// Heltec V3 OLED pins
#define OLED_SDA 17
#define OLED_SCL 18
#define OLED_RST 21
#define LED_PIN  35

SSD1306Wire display(0x3C, OLED_SDA, OLED_SCL);
int counter = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("Heltec WiFi LoRa 32 V3");

    pinMode(LED_PIN, OUTPUT);

    // Init OLED
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "Heltec LoRa V3");
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 20, "ESP32-S3 + SX1262");
    display.drawString(0, 35, "Initializing...");
    display.display();

    // Init WiFi
    WiFi.mode(WIFI_STA);
    Serial.printf("MAC: %s\n", WiFi.macAddress().c_str());

    delay(1000);
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 0, "Ready!");
    display.display();
}

void loop() {
    counter++;
    digitalWrite(LED_PIN, counter % 2);

    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.drawString(0, 0, "LoRa Counter");
    display.setFont(ArialMT_Plain_24);

    char buf[32];
    snprintf(buf, sizeof(buf), "#%d", counter);
    display.drawString(0, 25, buf);
    display.display();

    Serial.printf("Counter: %d\n", counter);
    delay(2000);
}
