// SCD4x CO2, temperature, humidity monitor example
// Demonstrates Sensirion SCD40/SCD41 sensor with periodic measurement

#include <Arduino.h>
#include <Wire.h>
#include <SensirionI2CScd4x.h>

SensirionI2CScd4x scd4x;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    scd4x.begin(Wire);

    // Get serial number
    uint16_t s0, s1, s2;
    scd4x.getSerialNumber(s0, s1, s2);
    Serial.print("SCD4x Serial: ");
    Serial.print(s0, HEX); Serial.print("-");
    Serial.print(s1, HEX); Serial.print("-");
    Serial.println(s2, HEX);

    // Configure
    scd4x.setTemperatureOffset(2.0f);
    scd4x.setSensorAltitude(450);  // Zurich ~450m

    // Start periodic measurement
    scd4x.startPeriodicMeasurement();

    Serial.println("SCD4x CO2 Monitor");
    Serial.println("==================");

    // Inject test values for emulation
    scd4x.test_setCO2(723);
    scd4x.test_setTemperature(23.1f);
    scd4x.test_setHumidity(42.8f);
    scd4x.test_setDataReady(true);
}

void loop() {
    bool ready = false;
    scd4x.getDataReadyFlag(ready);

    if (ready) {
        uint16_t co2;
        float temperature, humidity;

        if (scd4x.readMeasurement(co2, temperature, humidity) == 0) {
            Serial.print("CO2: ");
            Serial.print(co2);
            Serial.print(" ppm  |  Temp: ");
            Serial.print(temperature, 1);
            Serial.print(" °C  |  Humidity: ");
            Serial.print(humidity, 1);
            Serial.println(" %RH");

            if (co2 < 600) {
                Serial.println("  🌿 Excellent air quality");
            } else if (co2 < 1000) {
                Serial.println("  ✅ Good air quality");
            } else if (co2 < 1500) {
                Serial.println("  ⚠️  Moderate — open a window");
            } else {
                Serial.println("  🔴 Poor air quality — ventilate now!");
            }
        }
    }

    delay(5000);
}
