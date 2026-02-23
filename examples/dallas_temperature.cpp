// Dallas DS18B20 Temperature Sensor Example
// Reads temperature from 1-Wire DS18B20 sensors

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_PIN 4

OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

void setup() {
    Serial.begin(115200);
    Serial.println("Dallas DS18B20 Temperature Sensor");
    Serial.println("==================================");

    sensors.begin();
    Serial.print("Found ");
    Serial.print(sensors.getDeviceCount());
    Serial.println(" sensor(s)");

    sensors.setResolution(12);
    Serial.println("Resolution set to 12-bit");
}

void loop() {
    sensors.requestTemperatures();

    for (int i = 0; i < sensors.getDeviceCount(); i++) {
        float tempC = sensors.getTempCByIndex(i);
        float tempF = sensors.getTempFByIndex(i);
        Serial.print("Sensor ");
        Serial.print(i);
        Serial.print(": ");
        Serial.print(tempC);
        Serial.print("°C (");
        Serial.print(tempF);
        Serial.println("°F)");
    }

    Serial.println();
    delay(2000);
}
