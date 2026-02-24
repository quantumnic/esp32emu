#include <Arduino.h>
#include <HDC1080.h>

HDC1080 sensor;

void setup() {
    Serial.begin(115200);
    Serial.println("HDC1080 Climate Sensor");

    if (!sensor.begin()) {
        Serial.println("Failed to find HDC1080!");
        while (1) delay(10);
    }

    sensor.setResolution(HDC1080::RES_14BIT, HDC1080::RES_14BIT);

    Serial.print("Manufacturer ID: 0x");
    Serial.println(sensor.readManufacturerId(), HEX);
    Serial.print("Device ID: 0x");
    Serial.println(sensor.readDeviceId(), HEX);
}

void loop() {
    float temp = sensor.readTemperature();
    float hum = sensor.readHumidity();

    Serial.print("Temperature: ");
    Serial.print(temp, 1);
    Serial.print("°C  Humidity: ");
    Serial.print(hum, 1);
    Serial.println("%");

    delay(2000);
}
