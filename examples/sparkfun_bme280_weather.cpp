#include <Arduino.h>
#include <SparkFun_BME280.h>

BME280 sensor;

void setup() {
    Serial.begin(115200);
    sensor.settings.I2CAddress = 0x77;
    sensor.settings.runMode = BME280_MODE_NORMAL;
    sensor.settings.tempOverSample = BME280_OVERSAMP_16X;
    sensor.settings.pressOverSample = BME280_OVERSAMP_16X;
    sensor.settings.humidOverSample = BME280_OVERSAMP_16X;
    sensor.settings.filter = BME280_FILTER_4;
    sensor.settings.tStandby = BME280_STANDBY_500_MS;

    uint8_t chipID = sensor.begin();
    if (chipID != 0x60) {
        Serial.println("BME280 not found!");
        return;
    }
    Serial.println("SparkFun BME280 Weather Station");
}

void loop() {
    Serial.print("Temp: ");
    Serial.print(sensor.readTempC(), 1);
    Serial.print(" °C (");
    Serial.print(sensor.readTempF(), 1);
    Serial.println(" °F)");

    Serial.print("Humidity: ");
    Serial.print(sensor.readFloatHumidity(), 1);
    Serial.println(" %");

    Serial.print("Pressure: ");
    Serial.print(sensor.readFloatPressure() / 100.0f, 2);
    Serial.println(" hPa");

    Serial.print("Altitude: ");
    Serial.print(sensor.readFloatAltitudeMeters(), 1);
    Serial.print(" m (");
    Serial.print(sensor.readFloatAltitudeFeet(), 1);
    Serial.println(" ft)");

    Serial.println("---");
    delay(5000);
}
