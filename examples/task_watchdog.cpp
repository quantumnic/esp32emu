#include <Arduino.h>
#include "esp_task_wdt.h"

void setup() {
    Serial.begin(115200);
    Serial.println("Task Watchdog Timer Demo");

    // Initialize WDT with 5 second timeout
    esp_task_wdt_init(5000, true);
    esp_task_wdt_add(nullptr);
    Serial.println("WDT initialized (5s timeout)");
}

void loop() {
    // Simulate work
    Serial.println("Working... feeding watchdog");
    esp_task_wdt_reset();
    delay(1000);

    Serial.println("More work... feeding watchdog");
    esp_task_wdt_reset();
    delay(1000);

    Serial.println("Done with cycle, WDT happy!");
    delay(2000);
}
