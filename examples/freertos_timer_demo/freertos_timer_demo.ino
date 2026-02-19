// FreeRTOS Software Timer demo — periodic LED blink + one-shot buzzer
#include "freertos/timers.h"

static int ledState = 0;

void blinkCallback(TimerHandle_t xTimer) {
    ledState = !ledState;
    digitalWrite(2, ledState);
    Serial.print("LED: ");
    Serial.println(ledState ? "ON" : "OFF");
}

void buzzerCallback(TimerHandle_t xTimer) {
    Serial.println("BUZZ! (one-shot timer fired)");
}

void setup() {
    Serial.begin(115200);
    pinMode(2, OUTPUT);

    // Create a periodic 500ms blink timer
    TimerHandle_t blinkTimer = xTimerCreate("blink", pdMS_TO_TICKS(500), pdTRUE, nullptr, blinkCallback);
    xTimerStart(blinkTimer, 0);

    // Create a one-shot 2s buzzer timer
    TimerHandle_t buzzerTimer = xTimerCreate("buzzer", pdMS_TO_TICKS(2000), pdFALSE, nullptr, buzzerCallback);
    xTimerStart(buzzerTimer, 0);

    Serial.println("Timers started! Blink every 500ms, buzzer in 2s");
}

void loop() {
    delay(100);
}
