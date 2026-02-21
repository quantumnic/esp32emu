// esp32emu example — RCSwitch 433MHz RF Remote Control
#include <Arduino.h>
#include <RCSwitch.h>

RCSwitch transmitter;
RCSwitch receiver;

void setup() {
    Serial.begin(115200);
    Serial.println("=== 433MHz RF Control ===");

    transmitter.enableTransmit(4);  // TX on GPIO 4
    transmitter.setProtocol(1);
    transmitter.setRepeatTransmit(10);

    receiver.enableReceive(2);      // RX on GPIO 2
    Serial.println("✅ RF ready: TX=GPIO4, RX=GPIO2");
}

void loop() {
    // Send a code
    Serial.println("📡 Sending: 0x1A2B3C (24-bit)");
    transmitter.send(0x1A2B3C, 24);

    // Switch control (Type A)
    transmitter.switchOn("11001", "01000");
    Serial.println("💡 Switch ON");
    delay(2000);

    transmitter.switchOff("11001", "01000");
    Serial.println("💡 Switch OFF");
    delay(2000);

    // Check receiver
    if (receiver.available()) {
        unsigned long value = receiver.getReceivedValue();
        if (value == 0) {
            Serial.println("Unknown encoding");
        } else {
            Serial.printf("Received: 0x%lX (%d-bit, protocol %d)\n",
                          value, receiver.getReceivedBitlength(),
                          receiver.getReceivedProtocol());
        }
        receiver.resetAvailable();
    }

    delay(1000);
}
