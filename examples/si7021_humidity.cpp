// esp32emu example — Si7021 Temperature & Humidity Monitor
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Si7021.h>

Adafruit_Si7021 si7021;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!si7021.begin()) {
        Serial.println("Si7021 not found!");
        while (1) delay(10);
    }

    Serial.print("Si7021 revision: 0x");
    Serial.println(si7021.getRevision(), HEX);
    Serial.println("Si7021 ready.");
}

void loop() {
    float temp = si7021.readTemperature();
    float hum = si7021.readHumidity();

    Serial.print("Temperature: ");
    Serial.print(temp, 1);
    Serial.print(" °C  Humidity: ");
    Serial.print(hum, 1);
    Serial.println(" %");

    delay(2000);
}
