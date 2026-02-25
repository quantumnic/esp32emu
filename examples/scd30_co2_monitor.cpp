// esp32emu example — SCD30 CO2/temperature/humidity monitor
#include <Arduino.h>
#include <Adafruit_SCD30.h>

Adafruit_SCD30 scd30;

void setup() {
    Serial.begin(115200);
    if (!scd30.begin()) {
        Serial.println("SCD30 not found!");
        while (1) delay(10);
    }
    Serial.println("SCD30 CO2 sensor initialized");

    scd30.setMeasurementInterval(2);
    scd30.startContinuousMeasurement(1013);  // ambient pressure in mBar
    scd30.selfCalibrationEnabled(true);

    Serial.print("Measurement interval: ");
    Serial.print(scd30.getMeasurementInterval());
    Serial.println(" s");
}

void loop() {
    if (scd30.dataReady()) {
        if (scd30.read()) {
            Serial.print("CO2: ");
            Serial.print(scd30.CO2, 1);
            Serial.print(" ppm  Temp: ");
            Serial.print(scd30.temperature, 1);
            Serial.print(" °C  Humidity: ");
            Serial.print(scd30.relative_humidity, 1);
            Serial.println(" %");

            if (scd30.CO2 > 1000) {
                Serial.println("⚠️ CO2 level high — ventilate!");
            }
        }
    }
    delay(2000);
}
