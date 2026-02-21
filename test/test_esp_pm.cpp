// test_esp_pm: ESP-IDF power management mock tests
#include "esp_pm.h"
#include <cassert>
#include <cstdio>

int main() {
    esp32emu_pm_reset();

    // Configure PM
    esp_pm_config_t cfg = {240, 80, false};
    assert(esp_pm_configure(&cfg) == ESP_OK);

    // Read back
    esp_pm_config_t readback = {};
    assert(esp_pm_get_configuration(&readback) == ESP_OK);
    assert(readback.max_freq_mhz == 240);
    assert(readback.min_freq_mhz == 80);
    assert(readback.light_sleep_enable == false);

    // Invalid config
    esp_pm_config_t bad = {80, 240, false}; // min > max
    assert(esp_pm_configure(&bad) == ESP_ERR_INVALID_ARG);
    assert(esp_pm_configure(nullptr) == ESP_ERR_INVALID_ARG);

    // Light sleep
    cfg.light_sleep_enable = true;
    cfg.min_freq_mhz = 10;
    assert(esp_pm_configure(&cfg) == ESP_OK);
    assert(esp_pm_get_configuration(&readback) == ESP_OK);
    assert(readback.light_sleep_enable == true);

    // Locks
    esp_pm_lock_handle_t lock = nullptr;
    assert(esp_pm_lock_create(ESP_PM_CPU_FREQ_MAX, 0, "test_lock", &lock) == ESP_OK);
    assert(lock != nullptr);

    assert(esp32emu_pm_get_lock_count() == 0);
    assert(esp_pm_lock_acquire(lock) == ESP_OK);
    assert(esp32emu_pm_get_lock_count() == 1);
    assert(esp_pm_lock_acquire(lock) == ESP_OK);
    assert(esp32emu_pm_get_lock_count() == 2);
    assert(esp_pm_lock_release(lock) == ESP_OK);
    assert(esp32emu_pm_get_lock_count() == 1);
    assert(esp_pm_lock_release(lock) == ESP_OK);
    assert(esp32emu_pm_get_lock_count() == 0);

    // Extra release doesn't go negative
    assert(esp_pm_lock_release(lock) == ESP_OK);
    assert(esp32emu_pm_get_lock_count() == 0);

    assert(esp_pm_lock_delete(lock) == ESP_OK);

    printf("test_esp_pm: all assertions passed\n");
    return 0;
}
