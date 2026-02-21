// LoRa Receiver example — listens for packets on 868 MHz
#include "Arduino.h"
#include "LoRa.h"

void setup() {
    Serial.begin(115200);
    Serial.println("LoRa Receiver starting...");

    LoRa.setPins(5, 14, 2);
    if (!LoRa.begin(868E6)) {
        Serial.println("LoRa init failed!");
        while (1);
    }
    LoRa.setSpreadingFactor(7);
    LoRa.setSignalBandwidth(125E3);

    // Inject a test packet for demo
    LoRa.injectPacket("Hello #42", -65, 8.5f);

    Serial.println("LoRa listening...");
}

void loop() {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        Serial.print("Received packet (");
        Serial.print(packetSize);
        Serial.print(" bytes): ");
        while (LoRa.available()) {
            Serial.print((char)LoRa.read());
        }
        Serial.println();
        Serial.print("  RSSI: "); Serial.println(LoRa.packetRssi());
        Serial.print("  SNR: "); Serial.println(LoRa.packetSnr());
    }
    delay(100);
}
