// BME68x air quality monitoring example
#include <Arduino.h>
#include <Adafruit_BME68x.h>

Adafruit_BME68x bme;

void setup() {
    Serial.begin(115200);

    if (!bme.begin()) {
        Serial.println("BME68x not found!");
        while (1) delay(10);
    }

    // Configure oversampling and filter
    bme.setTemperatureOversampling(BME68X_OS_8X);
    bme.setPressureOversampling(BME68X_OS_4X);
    bme.setHumidityOversampling(BME68X_OS_2X);
    bme.setIIRFilterSize(BME68X_FILTER_SIZE_3);
    bme.setGasHeater(320, 150);

    Serial.println("BME68x Air Quality Monitor");
    Serial.println("==========================");
}

void loop() {
    if (!bme.performReading()) {
        Serial.println("Failed to read sensor!");
        delay(2000);
        return;
    }

    Serial.print("Temperature: ");
    Serial.print(bme.readTemperature());
    Serial.println(" °C");

    Serial.print("Pressure: ");
    Serial.print(bme.readPressure() / 100.0);
    Serial.println(" hPa");

    Serial.print("Humidity: ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.print("Gas: ");
    Serial.print(bme.readGas() / 1000.0);
    Serial.println(" kOhm");

    Serial.print("Altitude: ");
    Serial.print(bme.readAltitude(1013.25));
    Serial.println(" m");

    Serial.println("---");
    delay(2000);
}
