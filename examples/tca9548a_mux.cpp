// TCA9548A I2C multiplexer example — scan sensors on multiple I2C buses
#include <Arduino.h>
#include "TCA9548A.h"

TCA9548A mux(0x70);

void setup() {
    Serial.begin(115200);
    Serial.println("TCA9548A I2C Multiplexer");

    mux.begin();

    // Select each channel and "scan"
    for (uint8_t ch = 0; ch < 8; ch++) {
        mux.selectChannel(ch);
        Serial.print("Channel "); Serial.print(ch);
        Serial.print(" active: "); Serial.println(mux.isChannelActive(ch) ? "yes" : "no");
    }

    // Multi-channel mode
    mux.setChannelMask(0x05); // channels 0 + 2
    Serial.print("Mask: 0x"); Serial.println(mux.getChannelMask(), HEX);

    mux.closeAll();
    Serial.println("All channels closed.");
}

void loop() {
    delay(1000);
}
