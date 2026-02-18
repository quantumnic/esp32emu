// Deep Sleep Timer Example
// Demonstrates ESP32 deep sleep with timer wakeup.
// In emulation, sleep is intercepted (not exit) so tests can verify behavior.

#include <Arduino.h>
#include <esp_sleep.h>

#define uS_TO_S_FACTOR 1000000ULL
#define TIME_TO_SLEEP  10  // seconds

// RTC memory survives deep sleep (simulated as static)
static int bootCount = 0;

void printWakeupReason() {
    esp_sleep_wakeup_cause_t reason = esp_sleep_get_wakeup_cause();
    switch (reason) {
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("Wakeup: timer"); break;
        case ESP_SLEEP_WAKEUP_EXT0:
            Serial.println("Wakeup: external signal (ext0)"); break;
        case ESP_SLEEP_WAKEUP_TOUCHPAD:
            Serial.println("Wakeup: touchpad"); break;
        default:
            Serial.println("Wakeup: not from deep sleep"); break;
    }
}

void setup() {
    Serial.begin(115200);
    bootCount++;
    Serial.print("Boot count: ");
    Serial.println(bootCount);

    printWakeupReason();

    // Do some work
    Serial.println("Reading sensors...");
    float temperature = 23.5;
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    // Configure sleep
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    Serial.print("Going to deep sleep for ");
    Serial.print(TIME_TO_SLEEP);
    Serial.println(" seconds");

    esp_deep_sleep_start();
    // This line should never execute on real hardware
    Serial.println("(emulator: continued after deep sleep)");
}

void loop() {
    // Never reached on real hardware (deep sleep resets the chip)
    delay(1000);
}
