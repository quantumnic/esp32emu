// ESP32 Internal Temperature Sensor example
// Reads the on-chip temperature sensor periodically
#include <Arduino.h>
#include "driver/temperature_sensor.h"

temperature_sensor_handle_t temp_handle = nullptr;

void setup() {
    Serial.begin(115200);
    Serial.println("ESP32 Internal Temperature Sensor");

    temperature_sensor_config_t cfg = TEMPERATURE_SENSOR_CONFIG_DEFAULT(-10, 80);
    ESP_ERROR_CHECK(temperature_sensor_install(&cfg, &temp_handle));
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_handle));

    Serial.println("Temperature sensor initialized.");
}

void loop() {
    float temp_c;
    if (temperature_sensor_get_celsius(temp_handle, &temp_c) == ESP_OK) {
        float temp_f = temp_c * 9.0f / 5.0f + 32.0f;
        Serial.print("Chip temperature: ");
        Serial.print(temp_c, 1);
        Serial.print(" °C (");
        Serial.print(temp_f, 1);
        Serial.println(" °F)");

        if (temp_c > 70.0f) {
            Serial.println("⚠️  WARNING: High chip temperature!");
        }
    }
    delay(2000);
}
