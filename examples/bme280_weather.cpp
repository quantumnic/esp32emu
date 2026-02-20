#include <Arduino.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme;

void setup() {
    Serial.begin(115200);
    if (!bme.begin()) {
        Serial.println("BME280 not found!");
        return;
    }
    bme.setSampling(Adafruit_BME280::MODE_FORCED);
    Serial.println("BME280 Weather Station ready");
}

void loop() {
    bme.takeForcedMeasurement();
    Serial.print("Temperature: ");
    Serial.print(bme.readTemperature());
    Serial.println(" °C");
    Serial.print("Humidity: ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");
    Serial.print("Pressure: ");
    Serial.print(bme.readPressure() / 100.0f);
    Serial.println(" hPa");
    Serial.print("Altitude: ");
    Serial.print(bme.readAltitude(101325));
    Serial.println(" m");
    Serial.println("---");
    delay(2000);
}
