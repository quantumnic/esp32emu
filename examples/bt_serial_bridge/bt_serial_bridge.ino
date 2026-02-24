// esp32emu example: Bluetooth Serial bridge
// Bridges Bluetooth SPP to Serial (USB) — bidirectional data relay.

#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

void setup() {
    Serial.begin(115200);
    SerialBT.begin("ESP32-Bridge");
    Serial.println("Bluetooth Serial bridge started");
    Serial.println("Device name: ESP32-Bridge");
    Serial.println("Waiting for connection...");
}

void loop() {
    // Forward BT → Serial
    while (SerialBT.available()) {
        char c = SerialBT.read();
        Serial.write(c);
    }

    // Forward Serial → BT
    while (Serial.available()) {
        char c = Serial.read();
        SerialBT.write(c);
    }

    // Show connection status periodically
    static unsigned long lastStatus = 0;
    if (millis() - lastStatus > 10000) {
        if (SerialBT.hasClient()) {
            Serial.println("[Bridge] BT client connected");
        } else {
            Serial.println("[Bridge] Waiting for BT client...");
        }
        lastStatus = millis();
    }
}
