// LoRa Sender example — sends packets on 868 MHz
#include "Arduino.h"
#include "LoRa.h"

int counter = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("LoRa Sender starting...");

    LoRa.setPins(5, 14, 2);
    if (!LoRa.begin(868E6)) {
        Serial.println("LoRa init failed!");
        while (1);
    }
    LoRa.setSpreadingFactor(7);
    LoRa.setSignalBandwidth(125E3);
    LoRa.setTxPower(14);
    LoRa.enableCrc();

    Serial.println("LoRa initialized.");
}

void loop() {
    Serial.print("Sending packet #");
    Serial.println(counter);

    LoRa.beginPacket();
    LoRa.print("Hello #");
    char buf[16];
    snprintf(buf, sizeof(buf), "%d", counter);
    LoRa.print(buf);
    LoRa.endPacket();

    counter++;
    delay(5000);
}
