// esp32emu example — LPS22 barometric pressure sensor: altitude estimation
#include <Arduino.h>
#include <Adafruit_LPS2X.h>
#include <cmath>

Adafruit_LPS22 lps;

float pressureToAltitude(float pressure, float seaLevelPressure = 1013.25f) {
    return 44330.0f * (1.0f - pow(pressure / seaLevelPressure, 0.1903f));
}

void setup() {
    Serial.begin(115200);
    if (!lps.begin_I2C()) {
        Serial.println("LPS22 not found!");
        while (1) delay(10);
    }
    lps.setDataRate(LPS22_RATE_10_HZ);
    Serial.println("LPS22 barometer ready");
}

void loop() {
    sensors_event_t_lps2x pressure, temp;
    lps.getEvent(&pressure, &temp);

    float altitude = pressureToAltitude(pressure.pressure);
    Serial.printf("Pressure: %.2f hPa, Temp: %.1f °C, Alt: %.1f m\n",
                  pressure.pressure, temp.temperature, altitude);

    delay(1000);
}
