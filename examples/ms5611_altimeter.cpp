// esp32emu example: MS5611 Precision Altimeter
// Reads pressure, temperature, and calculates altitude.

#include <Arduino.h>
#include <Wire.h>
#include <MS5611.h>

MS5611 ms5611(0x77);
float seaLevelPa = 101325.0f;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!ms5611.begin()) {
        Serial.println("MS5611 not found!");
        while (1) delay(10);
    }

    ms5611.setOversampling(12); // highest precision
    ms5611.setReadings(22.5f, 1013.25f);

    Serial.println("=== MS5611 Altimeter ===");
}

void loop() {
    int result = ms5611.read();
    if (result != 0) {
        Serial.printf("Read error: %d\n", result);
        delay(1000);
        return;
    }

    float temp = ms5611.getTemperature();
    float press = ms5611.getPressure();
    float alt = ms5611.getAltitude(seaLevelPa);

    Serial.printf("Temp: %.2f°C | Press: %.2f mbar | Alt: %.1f m\n",
                  temp, press, alt);

    // Simulate altitude change
    float newPress = press - 0.12f; // ~1m climb
    ms5611.setPressure(newPress);

    delay(1000);
}
