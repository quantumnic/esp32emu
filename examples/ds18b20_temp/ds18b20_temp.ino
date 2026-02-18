// DS18B20 Temperature Sensor — Read temperature from OneWire sensor
// Demonstrates OneWire + DallasTemperature library usage

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_PIN 4

OneWire oneWire(ONE_WIRE_PIN);
DallasTemperature sensors(&oneWire);

void setup() {
    Serial.begin(115200);
    Serial.println("DS18B20 Temperature Sensor Demo");
    Serial.println("================================");

    // In emulator: add simulated sensor devices
    uint8_t addr1[8] = {0x28, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x01};
    uint8_t addr2[8] = {0x28, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x02};
    oneWire.addDevice(addr1);
    oneWire.addDevice(addr2);

    sensors.begin();
    sensors.setResolution(12);

    // Set simulated temperatures
    sensors.setTemperatureByIndex(0, 23.75);
    sensors.setTemperatureByIndex(1, 18.50);

    Serial.print("Found ");
    Serial.print(sensors.getDeviceCount());
    Serial.println(" sensor(s)");
}

void loop() {
    sensors.requestTemperatures();

    for (int i = 0; i < sensors.getDeviceCount(); i++) {
        float tempC = sensors.getTempCByIndex(i);
        float tempF = sensors.getTempFByIndex(i);

        Serial.print("Sensor ");
        Serial.print(i);
        Serial.print(": ");
        Serial.print(tempC, 2);
        Serial.print(" °C  (");
        Serial.print(tempF, 2);
        Serial.println(" °F)");
    }

    Serial.println("---");
    delay(2000);
}
