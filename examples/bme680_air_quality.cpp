#include <Arduino.h>
#include <Adafruit_BME680.h>

Adafruit_BME680 bme;

void setup() {
    Serial.begin(115200);
    if (!bme.begin()) {
        Serial.println("BME680 not found!");
        return;
    }
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150);
    Serial.println("BME680 Air Quality Monitor ready");
}

void loop() {
    if (!bme.performReading()) {
        Serial.println("Reading failed!");
        delay(2000);
        return;
    }
    Serial.print("Temperature: ");
    Serial.print(bme.temperature);
    Serial.println(" °C");
    Serial.print("Humidity: ");
    Serial.print(bme.humidity);
    Serial.println(" %");
    Serial.print("Pressure: ");
    Serial.print(bme.pressure / 100.0f);
    Serial.println(" hPa");
    Serial.print("Gas: ");
    Serial.print(bme.gas_resistance / 1000.0f);
    Serial.println(" kOhm");
    Serial.print("Altitude: ");
    Serial.print(bme.readAltitude(1013.25f));
    Serial.println(" m");
    Serial.println("---");
    delay(2000);
}
