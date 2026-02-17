// Button Input → LED Output with simulated button presses
// esp32emu run --board uno examples/button_led/button_led.ino
#include "Arduino.h"
#include "esp32emu_terminal.h"

#define BUTTON_PIN 7
#define LED_PIN 13

int buttonState = LOW;
int lastState = LOW;
bool ledOn = false;
unsigned long lastToggle = 0;

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    Serial.println("🔘 Button LED example — simulating button presses every 2s");
}

void loop() {
    // Simulate button press every 2 seconds
    if (millis() - lastToggle > 2000) {
        lastToggle = millis();
        int simVal = (millis() / 2000) % 2;
        esp32emu::GPIOController::instance().setAnalogInput(BUTTON_PIN, simVal ? 1023 : 0);
        // Also set digital for digitalRead
        if (simVal) {
            esp32emu::GPIOController::instance().digitalWrite(BUTTON_PIN, HIGH);
        } else {
            esp32emu::GPIOController::instance().digitalWrite(BUTTON_PIN, LOW);
        }
    }

    buttonState = digitalRead(BUTTON_PIN);

    if (buttonState != lastState) {
        if (buttonState == HIGH) {
            ledOn = !ledOn;
            digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
            esp32emu::printLED(LED_PIN, ledOn);
            Serial.print("Button PRESSED → LED ");
            Serial.println(ledOn ? "ON" : "OFF");
        } else {
            Serial.println("Button RELEASED");
        }
        lastState = buttonState;
    }

    delay(50); // debounce
}
