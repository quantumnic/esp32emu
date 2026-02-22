// TM1637 7-segment display clock example
#include "Arduino.h"
#include "TM1637Display.h"

TM1637Display display(18, 19);

void setup() {
    Serial.begin(115200);
    display.setBrightness(5);
    Serial.println("TM1637 clock display ready");
}

void loop() {
    // Show time 12:30 with colon
    display.showNumberDecEx(1230, 0x40, true);

    delay(500);

    // Blink colon off
    display.showNumberDecEx(1230, 0x00, true);

    delay(500);
}
