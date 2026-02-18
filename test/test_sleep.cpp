// Test ESP32 deep sleep / power management mock
#include "Arduino.h"
#include "esp_sleep.h"
#include <cassert>
#include <cstdio>

int main() {
    auto& sleep = esp32emu::SleepController::instance();
    sleep.reset();

    // Timer wakeup
    esp_sleep_enable_timer_wakeup(5000000); // 5 seconds
    assert(sleep.timerSet());
    assert(sleep.timerUs() == 5000000);

    // Ext0 wakeup
    esp_sleep_enable_ext0_wakeup(33, 1);
    assert(sleep.ext0Set());
    assert(sleep.ext0Pin() == 33);

    // Deep sleep (should NOT exit, just record)
    bool sleepCbCalled = false;
    sleep.onSleep([&]() { sleepCbCalled = true; });

    assert(!sleep.deepSleepCalled());
    esp_deep_sleep_start();
    assert(sleep.deepSleepCalled());
    assert(sleep.deepSleepCount() == 1);
    assert(sleepCbCalled);

    // Call again
    esp_deep_sleep_start();
    assert(sleep.deepSleepCount() == 2);

    // Light sleep
    assert(!sleep.lightSleepCalled());
    esp_light_sleep_start();
    assert(sleep.lightSleepCalled());
    assert(sleep.lightSleepCount() == 1);

    // Wakeup cause
    sleep.setWakeupCause(ESP_SLEEP_WAKEUP_TIMER);
    assert(esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER);

    sleep.setWakeupCause(ESP_SLEEP_WAKEUP_EXT0);
    assert(esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0);

    // Reset
    sleep.reset();
    assert(!sleep.deepSleepCalled());
    assert(!sleep.timerSet());
    assert(esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_UNDEFINED);

    printf("test_sleep: all assertions passed\n");
    return 0;
}
