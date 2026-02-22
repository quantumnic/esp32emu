// esp32emu — temperature_sensor driver test
#include "Arduino.h"
#include "driver/temperature_sensor.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    esp32emu::temp_sensor_reset();

    // Install
    temperature_sensor_config_t cfg = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);
    temperature_sensor_handle_t handle = nullptr;
    assert(temperature_sensor_install(&cfg, &handle) == ESP_OK);
    assert(handle != nullptr);

    // Can't read before enable
    float temp = 0;
    assert(temperature_sensor_get_celsius(handle, &temp) == ESP_ERR_INVALID_STATE);

    // Enable
    assert(temperature_sensor_enable(handle) == ESP_OK);

    // Read default (25°C)
    assert(temperature_sensor_get_celsius(handle, &temp) == ESP_OK);
    assert(fabs(temp - 25.0f) < 0.01f);

    // Inject
    esp32emu::temp_sensor_inject(42.5f);
    assert(temperature_sensor_get_celsius(handle, &temp) == ESP_OK);
    assert(fabs(temp - 42.5f) < 0.01f);

    // Disable
    assert(temperature_sensor_disable(handle) == ESP_OK);
    assert(temperature_sensor_get_celsius(handle, &temp) == ESP_ERR_INVALID_STATE);

    // Uninstall
    assert(temperature_sensor_uninstall(handle) == ESP_OK);

    // Invalid args
    assert(temperature_sensor_install(nullptr, &handle) == ESP_ERR_INVALID_ARG);
    assert(temperature_sensor_install(&cfg, nullptr) == ESP_ERR_INVALID_ARG);

    printf("test_temperature_sensor: all assertions passed\n");
    return 0;
}
