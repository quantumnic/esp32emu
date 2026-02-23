// esp32emu example: Adafruit INA219 Solar Panel Monitor
// Reads bus voltage, current, and power from an INA219 sensor.

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219(0x40);

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!ina219.begin()) {
        Serial.println("INA219 not found!");
        while (1) delay(10);
    }

    ina219.setCalibration_32V_2A();
    ina219.setReadings(13.8f, 1200.0f, 0.1f);

    Serial.println("=== Solar Panel Monitor ===");
}

void loop() {
    float busV = ina219.getBusVoltage_V();
    float current = ina219.getCurrent_mA();
    float power = ina219.getPower_mW();
    float shunt = ina219.getShuntVoltage_mV();

    Serial.printf("Bus: %.2fV | Current: %.0fmA | Power: %.0fmW | Shunt: %.2fmV\n",
                  busV, current, power, shunt);

    if (power > 10000) Serial.println("⚡ High power output!");
    delay(2000);
}
