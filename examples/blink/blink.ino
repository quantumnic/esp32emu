// Classic Arduino Blink — works on all boards
// esp32emu run --board uno examples/blink/blink.ino
#include "Arduino.h"
#include "esp32emu_terminal.h"

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("🔦 Blink example started!");
}

void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    esp32emu::printLED(LED_BUILTIN, true);
    Serial.println("LED ON");
    delay(1000);

    digitalWrite(LED_BUILTIN, LOW);
    esp32emu::printLED(LED_BUILTIN, false);
    Serial.println("LED OFF");
    delay(1000);
}
