#include <Arduino.h>
#include <Adafruit_MPL3115A2.h>

Adafruit_MPL3115A2 baro;

void setup() {
    Serial.begin(115200);
    Serial.println("MPL3115A2 Weather Station");

    if (!baro.begin()) {
        Serial.println("ERROR: MPL3115A2 not found!");
        while (1) delay(100);
    }

    baro.setOversampleRate(128);
    baro.setSeaPressure(101325.0f);
    Serial.println("Sensor ready.");
}

void loop() {
    float pressure = baro.getPressure();
    float altitude = baro.getAltitude();
    float temp = baro.getTemperature();

    Serial.print("Pressure: ");
    Serial.print(pressure / 100.0f, 1);
    Serial.print(" hPa  |  Altitude: ");
    Serial.print(altitude, 1);
    Serial.print(" m  |  Temp: ");
    Serial.print(temp, 1);
    Serial.println(" °C");

    delay(2000);
}
