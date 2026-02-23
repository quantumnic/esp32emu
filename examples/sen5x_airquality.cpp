// SEN55 air quality monitor — PM, VOC, NOx, temperature, humidity
#include <Arduino.h>
#include "SensirionI2CSen5x.h"

SensirionI2CSen5x sen;

void setup() {
    Serial.begin(115200);
    sen.begin();

    unsigned char serial[32];
    sen.getSerialNumber(serial, 32);
    Serial.print("SEN5x serial: ");
    Serial.println((char*)serial);

    sen.setTemperatureOffsetSimple(0.0f);
    sen.startMeasurement();
    Serial.println("Measuring air quality...");
}

void loop() {
    float pm1, pm25, pm4, pm10, hum, temp, voc, nox;
    if (sen.readMeasuredValues(pm1, pm25, pm4, pm10, hum, temp, voc, nox) == 0) {
        Serial.println("── Air Quality ──");
        Serial.print("PM1.0: "); Serial.print(pm1); Serial.println(" µg/m³");
        Serial.print("PM2.5: "); Serial.print(pm25); Serial.println(" µg/m³");
        Serial.print("PM10:  "); Serial.print(pm10); Serial.println(" µg/m³");
        Serial.print("Temp:  "); Serial.print(temp); Serial.println(" °C");
        Serial.print("Hum:   "); Serial.print(hum); Serial.println(" %");
        Serial.print("VOC:   "); Serial.println(voc);
        Serial.print("NOx:   "); Serial.println(nox);
    }
    delay(1000);
}
