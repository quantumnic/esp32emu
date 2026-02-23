// LoRa Receiver — listen for packets via SX1276
#include <Arduino.h>
#include <RadioLib.h>

Module mod(5, 2, 14);
SX1276 radio(&mod);

void setup() {
    Serial.begin(115200);
    int state = radio.begin(868.0, 125.0, 12, 5);
    if (state != RADIOLIB_ERR_NONE) {
        Serial.println("Radio init failed!");
        while (true) delay(1000);
    }
    Serial.println("LoRa Receiver ready, listening...");
    radio.startReceive();
}

void loop() {
    uint8_t buf[256];
    size_t len = sizeof(buf);
    int state = radio.receive(buf, len);
    if (state == RADIOLIB_ERR_NONE) {
        buf[len] = '\0';
        Serial.print("Received (");
        Serial.print(len);
        Serial.print(" bytes): ");
        Serial.println((char*)buf);
        Serial.print("  RSSI: ");
        Serial.print(radio.getRSSI());
        Serial.print(" dBm  SNR: ");
        Serial.print(radio.getSNR());
        Serial.println(" dB");
    }
    delay(100);
}
