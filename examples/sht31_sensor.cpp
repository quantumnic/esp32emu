// esp32emu example — SHT31 Temperature & Humidity Sensor
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SHT31.h>

Adafruit_SHT31 sht31;

void setup() {
    Serial.begin(115200);
    Serial.println("SHT31 Sensor Example");

    Wire.begin();

    if (!sht31.begin(0x44)) {
        Serial.println("ERROR: SHT31 not found!");
        while (1) delay(1000);
    }

    Serial.print("Heater enabled: ");
    Serial.println(sht31.isHeaterEnabled() ? "YES" : "NO");
}

void loop() {
    float t = sht31.readTemperature();
    float h = sht31.readHumidity();

    if (!isnan(t)) {
        Serial.print("Temperature: "); Serial.print(t); Serial.println(" °C");
    }
    if (!isnan(h)) {
        Serial.print("Humidity:    "); Serial.print(h); Serial.println(" %");
    }
    Serial.println("---");

    delay(2000);
}
