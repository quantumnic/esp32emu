#pragma once
// esp32emu: ESP-IDF power management mock
#include "esp_err.h"
#include <cstdint>
#include <cstdio>

typedef enum {
    ESP_PM_CPU_FREQ_MAX,
    ESP_PM_APB_FREQ_MAX,
    ESP_PM_NO_LIGHT_SLEEP,
} esp_pm_lock_type_t;

typedef void* esp_pm_lock_handle_t;

typedef struct {
    int max_freq_mhz;
    int min_freq_mhz;
    bool light_sleep_enable;
} esp_pm_config_esp32_t;

// Also used as generic config
typedef esp_pm_config_esp32_t esp_pm_config_t;

namespace esp32emu_pm {
    inline esp_pm_config_t current_config = {240, 80, false};
    inline int lock_count = 0;
}

inline esp_err_t esp_pm_configure(const esp_pm_config_t* config) {
    if (!config) return ESP_ERR_INVALID_ARG;
    if (config->min_freq_mhz > config->max_freq_mhz) return ESP_ERR_INVALID_ARG;
    esp32emu_pm::current_config = *config;
    printf("[esp32emu] PM: configured max=%dMHz min=%dMHz light_sleep=%d\n",
           config->max_freq_mhz, config->min_freq_mhz, config->light_sleep_enable);
    return ESP_OK;
}

inline esp_err_t esp_pm_get_configuration(esp_pm_config_t* config) {
    if (!config) return ESP_ERR_INVALID_ARG;
    *config = esp32emu_pm::current_config;
    return ESP_OK;
}

inline esp_err_t esp_pm_lock_create(esp_pm_lock_type_t lock_type, int /*arg*/, const char* name, esp_pm_lock_handle_t* out_handle) {
    printf("[esp32emu] PM: lock created type=%d name=%s\n", lock_type, name ? name : "");
    static int dummy = 0;
    *out_handle = &dummy;
    return ESP_OK;
}

inline esp_err_t esp_pm_lock_acquire(esp_pm_lock_handle_t handle) {
    if (!handle) return ESP_ERR_INVALID_ARG;
    esp32emu_pm::lock_count++;
    return ESP_OK;
}

inline esp_err_t esp_pm_lock_release(esp_pm_lock_handle_t handle) {
    if (!handle) return ESP_ERR_INVALID_ARG;
    if (esp32emu_pm::lock_count > 0) esp32emu_pm::lock_count--;
    return ESP_OK;
}

inline esp_err_t esp_pm_lock_delete(esp_pm_lock_handle_t handle) {
    if (!handle) return ESP_ERR_INVALID_ARG;
    return ESP_OK;
}

// esp32emu test helpers
inline int esp32emu_pm_get_lock_count() { return esp32emu_pm::lock_count; }
inline void esp32emu_pm_reset() {
    esp32emu_pm::current_config = {240, 80, false};
    esp32emu_pm::lock_count = 0;
}
