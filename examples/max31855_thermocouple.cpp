// MAX31855 thermocouple reader — K-type temperature monitoring
#include <Arduino.h>
#include <Adafruit_MAX31855.h>

#define CLK_PIN  13
#define CS_PIN   10
#define MISO_PIN 12

Adafruit_MAX31855 thermocouple(CLK_PIN, CS_PIN, MISO_PIN);

void setup() {
    Serial.begin(115200);
    Serial.println("MAX31855 Thermocouple Test");
    if (!thermocouple.begin()) {
        Serial.println("ERROR: MAX31855 not found!");
        while (1) delay(100);
    }
    Serial.println("Sensor ready.");
}

void loop() {
    double c = thermocouple.readCelsius();
    if (isnan(c)) {
        uint8_t err = thermocouple.readError();
        Serial.print("FAULT: ");
        if (err & Adafruit_MAX31855::MAX31855_FAULT_OPEN) Serial.print("OPEN ");
        if (err & Adafruit_MAX31855::MAX31855_FAULT_SHORT_GND) Serial.print("SHORT_GND ");
        if (err & Adafruit_MAX31855::MAX31855_FAULT_SHORT_VCC) Serial.print("SHORT_VCC ");
        Serial.println();
    } else {
        Serial.print("Thermocouple: "); Serial.print(c); Serial.print("°C / ");
        Serial.print(thermocouple.readFahrenheit()); Serial.println("°F");
        Serial.print("Internal:     "); Serial.print(thermocouple.readInternal());
        Serial.println("°C");
    }
    delay(1000);
}
