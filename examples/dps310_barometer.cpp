// DPS310 barometric pressure sensor — temperature, pressure, altitude
#include <Arduino.h>
#include "DPS310.h"

Dps310 sensor;

void setup() {
    Serial.begin(115200);
    if (sensor.begin() != 0) {
        Serial.println("DPS310 not found!");
        while (1) delay(100);
    }
    Serial.println("DPS310 ready — reading environment");
}

void loop() {
    float temp, pressure;
    if (sensor.measureTempOnce(temp) == 0 && sensor.measurePressureOnce(pressure) == 0) {
        float altitude = sensor.getAltitude();
        Serial.print("Temp: "); Serial.print(temp, 1); Serial.print(" °C  ");
        Serial.print("Pressure: "); Serial.print(pressure, 2); Serial.print(" hPa  ");
        Serial.print("Altitude: "); Serial.print(altitude, 1); Serial.println(" m");
    }
    delay(1000);
}
