// esp32emu example — ESP32 Internal Temperature Sensor
// Read the built-in temperature sensor using ESP-IDF driver.
#include "Arduino.h"
#include "driver/temperature_sensor.h"

temperature_sensor_handle_t temp_handle = nullptr;

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32 Internal Temperature Sensor");

    temperature_sensor_config_t cfg = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);
    ESP_ERROR_CHECK(temperature_sensor_install(&cfg, &temp_handle));
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_handle));

    // Inject for demo
    esp32emu::temp_sensor_inject(45.2f);
}

void loop() {
    float temp;
    if (temperature_sensor_get_celsius(temp_handle, &temp) == ESP_OK) {
        Serial.printf("Internal temp: %.1f°C\n", temp);
        if (temp > 80) Serial.println("⚠️  WARNING: High temperature!");
    }
    delay(1000);
}
