/*
 * BMP280 Weather Station — Read temperature, pressure, and altitude.
 * Uses Adafruit BMP280 library over I2C.
 */

#include <Wire.h>
#include <Adafruit_BMP280.h>

Adafruit_BMP280 bmp;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!bmp.begin(0x76)) {
        Serial.println("ERROR: BMP280 not found!");
        while (true) delay(1000);
    }

    Serial.println("BMP280 Weather Station");
    Serial.println("======================");

    // Simulate some weather conditions
    bmp.setTemperature(22.5);
    bmp.setPressure(101100.0);  // Slightly above sea level
}

int reading = 0;

void loop() {
    float temp = bmp.readTemperature();
    float pres = bmp.readPressure();
    float alt  = bmp.readAltitude(1013.25);

    Serial.print("Reading #");
    Serial.println(reading);
    Serial.print("  Temperature: ");
    Serial.print(temp, 1);
    Serial.println(" °C");
    Serial.print("  Pressure:    ");
    Serial.print(pres / 100.0, 2);
    Serial.println(" hPa");
    Serial.print("  Altitude:    ");
    Serial.print(alt, 1);
    Serial.println(" m");
    Serial.println();

    // Simulate temperature rising
    bmp.setTemperature(temp + 0.5);
    bmp.setPressure(pres - 50.0);

    reading++;
    delay(2000);

    if (reading >= 5) {
        Serial.println("Done!");
        while (true) delay(1000);
    }
}
