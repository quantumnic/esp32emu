#include <Arduino.h>
#include <Adafruit_INA228.h>

Adafruit_INA228 ina228;

void setup() {
    Serial.begin(115200);
    Serial.println("INA228 Power Monitor");

    if (!ina228.begin()) {
        Serial.println("Failed to find INA228!");
        while (1) delay(10);
    }
    ina228.setShunt(0.015, 10.0);
    ina228.setAveragingCount(128);
    Serial.println("INA228 initialized");
}

void loop() {
    float bus_v = ina228.readBusVoltage();
    float current = ina228.readCurrent();
    float power = ina228.readPower();
    float energy = ina228.readEnergy();

    Serial.print("Bus: "); Serial.print(bus_v, 2); Serial.println(" V");
    Serial.print("Current: "); Serial.print(current, 3); Serial.println(" A");
    Serial.print("Power: "); Serial.print(power, 2); Serial.println(" W");
    Serial.print("Energy: "); Serial.print(energy, 2); Serial.println(" J");

    if (ina228.mathOverflow()) {
        Serial.println("⚠️ Math overflow detected!");
    }

    Serial.println("---");
    delay(1000);
}
