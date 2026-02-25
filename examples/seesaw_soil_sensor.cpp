#include <Arduino.h>
#include <Adafruit_seesaw.h>

Adafruit_seesaw soilSensor;

void setup() {
    Serial.begin(115200);
    if (!soilSensor.begin(0x36)) {
        Serial.println("STEMMA Soil Sensor not found!");
        return;
    }
    Serial.print("Seesaw version: 0x");
    Serial.println(soilSensor.getVersion(), HEX);
    Serial.println("Soil sensor ready");
}

void loop() {
    float temp = soilSensor.getTemp();
    uint16_t moisture = soilSensor.touchRead(0);

    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.print(" °C  Moisture: ");
    Serial.println(moisture);

    if (moisture < 400) {
        Serial.println("⚠️ Soil is dry — water your plant!");
    } else if (moisture > 800) {
        Serial.println("💧 Soil is well watered");
    }
    delay(2000);
}
