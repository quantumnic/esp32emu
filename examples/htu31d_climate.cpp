#include <Arduino.h>
#include <Adafruit_HTU31D.h>

Adafruit_HTU31D htu;

void setup() {
    Serial.begin(115200);
    Serial.println("HTU31D Temperature & Humidity Sensor");

    if (!htu.begin()) {
        Serial.println("Failed to find HTU31D!");
        while (1) delay(10);
    }

    Serial.print("Sensor serial: 0x");
    Serial.println(htu.readSerial(), HEX);
    Serial.println("HTU31D initialized");
}

void loop() {
    float temp = htu.readTemperature();
    float hum = htu.readHumidity();

    // Dew point approximation (Magnus formula)
    float gamma = (17.62f * temp) / (243.12f + temp) + log(hum / 100.0f);
    float dewpoint = (243.12f * gamma) / (17.62f - gamma);

    Serial.print("Temperature: "); Serial.print(temp, 1); Serial.println(" °C");
    Serial.print("Humidity: "); Serial.print(hum, 1); Serial.println(" %");
    Serial.print("Dew Point: "); Serial.print(dewpoint, 1); Serial.println(" °C");

    if (!htu.diagnosticStatus()) {
        Serial.println("⚠️ Sensor diagnostic error!");
    }

    Serial.println("---");
    delay(2000);
}
