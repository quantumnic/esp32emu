#pragma once
// esp32emu — ESP32 internal temperature sensor driver mock
// Supports ESP-IDF temperature_sensor API for host-side testing.

#include "esp_err.h"
#include <cstdio>
#include <cstdint>

typedef struct temperature_sensor_obj_t* temperature_sensor_handle_t;

struct temperature_sensor_config_t {
    int range_min;
    int range_max;
    int clk_src;  // 0 = default
};

#define TEMPERATURE_SENSOR_CONFIG_DEFAULT(min, max) \
    { .range_min = (min), .range_max = (max), .clk_src = 0 }

namespace esp32emu {

struct TempSensorState {
    bool installed = false;
    bool enabled = false;
    float inject_temp = 25.0f;  // Default 25°C
    temperature_sensor_config_t config = {};
};

inline TempSensorState& temp_sensor_state() {
    static TempSensorState s;
    return s;
}

// Test helper: inject temperature reading
inline void temp_sensor_inject(float celsius) {
    temp_sensor_state().inject_temp = celsius;
}

inline void temp_sensor_reset() {
    temp_sensor_state() = TempSensorState{};
}

} // namespace esp32emu

// Opaque handle — we use a sentinel pointer
struct temperature_sensor_obj_t {};
inline temperature_sensor_obj_t g_temp_sensor_obj;

inline esp_err_t temperature_sensor_install(const temperature_sensor_config_t* config,
                                             temperature_sensor_handle_t* handle) {
    if (!config || !handle) return ESP_ERR_INVALID_ARG;
    auto& s = esp32emu::temp_sensor_state();
    s.config = *config;
    s.installed = true;
    *handle = &g_temp_sensor_obj;
    fprintf(stderr, "[esp32emu] Temperature sensor installed (range %d..%d°C)\n",
            config->range_min, config->range_max);
    return ESP_OK;
}

inline esp_err_t temperature_sensor_uninstall(temperature_sensor_handle_t handle) {
    if (!handle) return ESP_ERR_INVALID_ARG;
    esp32emu::temp_sensor_state().installed = false;
    esp32emu::temp_sensor_state().enabled = false;
    return ESP_OK;
}

inline esp_err_t temperature_sensor_enable(temperature_sensor_handle_t handle) {
    if (!handle) return ESP_ERR_INVALID_ARG;
    esp32emu::temp_sensor_state().enabled = true;
    return ESP_OK;
}

inline esp_err_t temperature_sensor_disable(temperature_sensor_handle_t handle) {
    if (!handle) return ESP_ERR_INVALID_ARG;
    esp32emu::temp_sensor_state().enabled = false;
    return ESP_OK;
}

inline esp_err_t temperature_sensor_get_celsius(temperature_sensor_handle_t handle, float* out) {
    if (!handle || !out) return ESP_ERR_INVALID_ARG;
    auto& s = esp32emu::temp_sensor_state();
    if (!s.enabled) return ESP_ERR_INVALID_STATE;
    *out = s.inject_temp;
    return ESP_OK;
}
