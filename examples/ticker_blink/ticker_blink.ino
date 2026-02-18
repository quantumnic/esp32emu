// Ticker Blink — Non-blocking LED blink using Ticker library
// Demonstrates periodic callbacks without blocking loop()

#include <Arduino.h>
#include <Ticker.h>

Ticker ledTicker;
Ticker statusTicker;

volatile bool ledState = false;
volatile int tickCount = 0;

void toggleLED() {
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
    tickCount++;
}

void printStatus() {
    Serial.print("Tick count: ");
    Serial.println(tickCount);
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    // Blink LED every 0.5 seconds
    ledTicker.attach(0.5, toggleLED);

    // Print status every 2 seconds
    statusTicker.attach(2.0, printStatus);

    Serial.println("Ticker blink started!");
}

void loop() {
    // loop is free for other work
    delay(100);

    // Stop after 10 ticks
    if (tickCount >= 10) {
        ledTicker.detach();
        statusTicker.detach();
        Serial.println("Done! Tickers detached.");
        delay(1000);
    }
}
