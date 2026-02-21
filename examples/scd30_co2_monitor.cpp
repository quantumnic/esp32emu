// esp32emu example: SCD30 CO2/Temperature/Humidity Monitor
// Reads CO2, temperature and humidity from Sensirion SCD30.

#include <Arduino.h>
#include <Wire.h>
#include <SCD30.h>

SCD30 scd30;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!scd30.begin()) {
        Serial.println("SCD30 not found!");
        while (1) delay(10);
    }

    scd30.setMeasurementInterval(2);
    scd30.setAutoSelfCalibration(true);
    scd30.setTemperatureOffset(0.0f);
    scd30.beginMeasuring();

    // Test: simulate office environment
    scd30.setReadings(650.0f, 23.5f, 42.0f);

    Serial.println("SCD30 CO2 Monitor");
}

void loop() {
    if (scd30.dataAvailable()) {
        Serial.print("CO2: "); Serial.print(scd30.getCO2(), 0); Serial.println(" ppm");
        Serial.print("Temp: "); Serial.print(scd30.getTemperature(), 1); Serial.println(" °C");
        Serial.print("Humidity: "); Serial.print(scd30.getHumidity(), 1); Serial.println(" %");
        Serial.println("---");
    }
    delay(2000);
}
