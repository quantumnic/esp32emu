// esp32emu example — BME680 Air Quality Monitor
// Reads temperature, humidity, pressure, and gas resistance for IAQ.
#include <Arduino.h>
#include <Adafruit_BME680.h>

Adafruit_BME680 bme;

void setup() {
    Serial.begin(115200);
    Serial.println("BME680 Air Quality Monitor");

    if (!bme.begin()) {
        Serial.println("ERROR: BME680 not found!");
        while (1) delay(1000);
    }

    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(320, 150);

    Serial.println("Sensor initialized.");
}

void loop() {
    if (!bme.performReading()) {
        Serial.println("Reading failed!");
        delay(2000);
        return;
    }

    Serial.print("Temperature: "); Serial.print(bme.temperature); Serial.println(" °C");
    Serial.print("Humidity:    "); Serial.print(bme.humidity);    Serial.println(" %");
    Serial.print("Pressure:    "); Serial.print(bme.pressure / 100.0); Serial.println(" hPa");
    Serial.print("Gas:         "); Serial.print(bme.gas_resistance / 1000.0); Serial.println(" kΩ");
    Serial.print("Altitude:    "); Serial.print(bme.readAltitude()); Serial.println(" m");
    Serial.println("---");

    delay(2000);
}
