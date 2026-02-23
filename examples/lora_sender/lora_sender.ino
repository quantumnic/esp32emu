// LoRa Sender — transmit sensor data via SX1276
#include <Arduino.h>
#include <RadioLib.h>

Module mod(5, 2, 14);
SX1276 radio(&mod);

int counter = 0;

void setup() {
    Serial.begin(115200);
    int state = radio.begin(868.0, 125.0, 12, 5);
    if (state != RADIOLIB_ERR_NONE) {
        Serial.println("Radio init failed!");
        while (true) delay(1000);
    }
    radio.setOutputPower(14);
    Serial.println("LoRa Sender ready");
}

void loop() {
    char msg[64];
    snprintf(msg, sizeof(msg), "Hello #%d temp=%.1f", counter++, 22.5 + (counter % 10) * 0.3);

    int state = radio.transmit(msg);
    if (state == RADIOLIB_ERR_NONE) {
        Serial.print("Sent: ");
        Serial.println(msg);
    } else {
        Serial.print("TX failed, code: ");
        Serial.println(state);
    }
    delay(5000);
}
