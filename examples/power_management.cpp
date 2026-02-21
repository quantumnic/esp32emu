// Example: ESP-IDF power management configuration
#include <Arduino.h>
#include "esp_pm.h"
#include <cstdio>

void setup() {
    printf("ESP32 Power Management Demo\n");
    printf("===========================\n");

    // Configure PM: 240MHz max, 80MHz min, no light sleep
    esp_pm_config_t pm_config = {
        .max_freq_mhz = 240,
        .min_freq_mhz = 80,
        .light_sleep_enable = false
    };

    if (esp_pm_configure(&pm_config) == ESP_OK) {
        printf("PM configured: %d-%d MHz\n", pm_config.min_freq_mhz, pm_config.max_freq_mhz);
    }

    // Create a CPU frequency lock for high-performance sections
    esp_pm_lock_handle_t cpu_lock;
    esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "cpu_max", &cpu_lock);

    // Acquire lock during intensive computation
    printf("Acquiring CPU lock for computation...\n");
    esp_pm_lock_acquire(cpu_lock);

    // ... do intensive work ...
    delay(100);

    printf("Releasing CPU lock\n");
    esp_pm_lock_release(cpu_lock);
    esp_pm_lock_delete(cpu_lock);

    // Enable light sleep
    pm_config.light_sleep_enable = true;
    pm_config.min_freq_mhz = 10;
    esp_pm_configure(&pm_config);
    printf("Light sleep enabled, min freq: %dMHz\n", pm_config.min_freq_mhz);
}

void loop() {
    delay(5000);
}
