// Classic Arduino blink sketch — runs on your Mac via esp32emu
#include "Arduino.h"

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("Blink example started!");
}

void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("LED ON  (pin 2 = HIGH)");
    delay(1000);

    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("LED OFF (pin 2 = LOW)");
    delay(1000);
}
