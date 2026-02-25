// esp32emu example — MAX31856 Precision Thermocouple Reader
#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_MAX31856.h>

#define CS_PIN 5

Adafruit_MAX31856 maxthermo(CS_PIN);

void setup() {
    Serial.begin(115200);

    if (!maxthermo.begin()) {
        Serial.println("MAX31856 not found!");
        while (1) delay(10);
    }

    maxthermo.setThermocoupleType(MAX31856_TCTYPE_K);
    maxthermo.setConversionMode(MAX31856_CONTINUOUS);

    Serial.println("MAX31856 ready — Type K thermocouple");
}

void loop() {
    float tcTemp = maxthermo.readThermocoupleTemperature();
    float cjTemp = maxthermo.readCJTemperature();

    Serial.print("Thermocouple: ");
    Serial.print(tcTemp, 1);
    Serial.print(" °C  Cold Junction: ");
    Serial.print(cjTemp, 1);
    Serial.println(" °C");

    uint8_t fault = maxthermo.readFault();
    if (fault) {
        Serial.print("FAULT: 0x");
        Serial.println(fault, HEX);
    }

    delay(1000);
}
