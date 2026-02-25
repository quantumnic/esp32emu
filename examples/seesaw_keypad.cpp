#include <Arduino.h>
#include <Adafruit_seesaw.h>

Adafruit_seesaw keypad;

void setup() {
    Serial.begin(115200);
    if (!keypad.begin(0x49)) {
        Serial.println("Seesaw keypad not found!");
        return;
    }
    Serial.println("Seesaw NeoTrellis keypad ready");
    keypad.enableKeypadInterrupt();

    // Enable events for 16 keys (4x4)
    for (uint8_t i = 0; i < 16; i++) {
        keypad.setKeypadEvent(i, SEESAW_KEYPAD_EDGE_RISING, true);
        keypad.setKeypadEvent(i, SEESAW_KEYPAD_EDGE_FALLING, true);
    }
}

void loop() {
    uint8_t count = keypad.getKeypadCount();
    if (count > 0) {
        keyEventRaw events[16];
        keypad.readKeypad(events, count);
        for (uint8_t i = 0; i < count; i++) {
            Serial.print("Key ");
            Serial.print(events[i].bit.NUM);
            if (events[i].bit.EDGE == SEESAW_KEYPAD_EDGE_RISING) {
                Serial.println(" pressed");
            } else {
                Serial.println(" released");
            }
        }
    }
    delay(20);
}
