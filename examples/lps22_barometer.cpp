#include <Arduino.h>
#include <Adafruit_LPS22.h>

Adafruit_LPS22 lps;

void setup() {
    Serial.begin(115200);
    Serial.println("LPS22 Barometric Pressure Sensor");

    if (!lps.begin_I2C()) {
        Serial.println("Failed to find LPS22!");
        while (1) delay(10);
    }

    lps.setDataRate(LPS22_RATE_10_HZ);
    Serial.println("LPS22 initialized @ 10 Hz");
}

void loop() {
    if (lps.pressureAvailable()) {
        float pressure = lps.readPressure();
        float temperature = lps.readTemperature();

        // Altitude from barometric formula (ISA)
        float altitude = 44330.0 * (1.0 - pow(pressure / 1013.25, 0.1903));

        Serial.print("Pressure: "); Serial.print(pressure, 2); Serial.println(" hPa");
        Serial.print("Temperature: "); Serial.print(temperature, 1); Serial.println(" °C");
        Serial.print("Altitude: "); Serial.print(altitude, 1); Serial.println(" m");
        Serial.println("---");
    }

    delay(1000);
}
