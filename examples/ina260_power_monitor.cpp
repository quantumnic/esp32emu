// esp32emu example: INA260 Precision Power Monitor
// Reads voltage, current, and power from an INA260 sensor.

#include <Arduino.h>
#include <Wire.h>
#include <INA260.h>

INA260 ina260;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!ina260.begin(0x40)) {
        Serial.println("INA260 not found!");
        while (1) delay(10);
    }

    ina260.setAveragingCount(INA260_AveragingCount::AVG_16);
    ina260.setVoltageConversionTime(INA260_ConversionTime::TIME_1_1_ms);
    ina260.setCurrentConversionTime(INA260_ConversionTime::TIME_1_1_ms);

    // Test: USB-C device drawing 500mA at 5V
    ina260.setReadings(5.0f, 500.0f);

    Serial.println("INA260 Power Monitor");
}

void loop() {
    Serial.print("Voltage: "); Serial.print(ina260.readBusVoltage(), 2); Serial.println(" V");
    Serial.print("Current: "); Serial.print(ina260.readCurrent(), 1); Serial.println(" mA");
    Serial.print("Power:   "); Serial.print(ina260.readPower(), 1); Serial.println(" mW");
    Serial.println("---");
    delay(1000);
}
