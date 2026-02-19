// esp32emu test — temperature sensor driver
#include "driver/temperature_sensor.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    esp32emu::temp_sensor_reset();

    temperature_sensor_handle_t handle = nullptr;
    temperature_sensor_config_t cfg = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);

    // Install
    assert(temperature_sensor_install(&cfg, &handle) == ESP_OK);
    assert(handle != nullptr);

    // Reading before enable should fail
    float temp = 0;
    assert(temperature_sensor_get_celsius(handle, &temp) == ESP_ERR_INVALID_STATE);

    // Enable
    assert(temperature_sensor_enable(handle) == ESP_OK);

    // Default reading is 25°C
    assert(temperature_sensor_get_celsius(handle, &temp) == ESP_OK);
    assert(fabs(temp - 25.0f) < 0.01f);

    // Inject temperature
    esp32emu::temp_sensor_inject(42.5f);
    assert(temperature_sensor_get_celsius(handle, &temp) == ESP_OK);
    assert(fabs(temp - 42.5f) < 0.01f);

    // Inject negative
    esp32emu::temp_sensor_inject(-5.0f);
    assert(temperature_sensor_get_celsius(handle, &temp) == ESP_OK);
    assert(fabs(temp - (-5.0f)) < 0.01f);

    // Disable and verify read fails
    assert(temperature_sensor_disable(handle) == ESP_OK);
    assert(temperature_sensor_get_celsius(handle, &temp) == ESP_ERR_INVALID_STATE);

    // Uninstall
    assert(temperature_sensor_uninstall(handle) == ESP_OK);

    printf("test_tempsensor: all assertions passed\n");
    return 0;
}
