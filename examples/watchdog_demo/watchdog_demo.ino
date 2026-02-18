// Watchdog Timer Demo — ESP32 Task Watchdog Timer usage
// Demonstrates feeding the watchdog to prevent reset

#include <Arduino.h>
#include <esp_task_wdt.h>

#define WDT_TIMEOUT_MS 3000

int loopCount = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32 Watchdog Timer Demo");
    Serial.println("=========================");

    // Initialize watchdog with 3 second timeout
    esp_task_wdt_init(WDT_TIMEOUT_MS, true);
    esp_task_wdt_add(NULL); // Subscribe current task

    Serial.println("Watchdog initialized (3s timeout)");
}

void loop() {
    loopCount++;
    Serial.print("Loop ");
    Serial.print(loopCount);
    Serial.println(": feeding watchdog...");

    // Feed the watchdog - prevents reset
    esp_task_wdt_reset();

    // Simulate work
    delay(1000);

    if (loopCount >= 5) {
        Serial.println("Demo complete! Removing watchdog subscription.");
        esp_task_wdt_delete(NULL);
        esp_task_wdt_deinit();
        Serial.println("Watchdog disabled. Goodbye!");
        delay(1000);
    }
}
