// Example: MAX6675 thermocouple temperature reading
#include <Arduino.h>
#include "MAX6675.h"
#include <cstdio>

MAX6675 thermocouple(13, 15, 12); // SCLK, CS, MISO

void setup() {
    printf("MAX6675 Thermocouple Reader\n");
    printf("===========================\n");
    // In emulation, set a test temperature
    thermocouple.setTemperature(185.5f);
}

void loop() {
    float c = thermocouple.readCelsius();
    float f = thermocouple.readFahrenheit();
    printf("Temperature: %.1f°C / %.1f°F\n", c, f);
    delay(1000);
}
