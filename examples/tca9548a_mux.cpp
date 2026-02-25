// esp32emu example — TCA9548A I2C multiplexer: scan devices on multiple buses
#include <Arduino.h>
#include <Adafruit_TCA9548A.h>

Adafruit_TCA9548A mux;

void setup() {
    Serial.begin(115200);
    if (!mux.begin()) {
        Serial.println("TCA9548A not found!");
        while (1) delay(10);
    }
    Serial.println("TCA9548A I2C multiplexer ready");

    // Scan each channel
    for (uint8_t ch = 0; ch < 8; ch++) {
        mux.selectChannel(ch);
        Serial.printf("Channel %u selected (scanning...)\n", ch);
        delay(50);
    }
    mux.disableAll();
    Serial.println("All channels disabled");
}

void loop() {
    // Cycle through channels
    for (uint8_t ch = 0; ch < 8; ch++) {
        mux.selectChannel(ch);
        Serial.printf("Reading from channel %u\n", ch);
        delay(500);
    }
}
