// esp32emu example — Adafruit VL53L0X Time-of-Flight Distance Sensor
#include <Arduino.h>
#include <Adafruit_VL53L0X.h>

Adafruit_VL53L0X lox;

void setup() {
    Serial.begin(115200);
    Serial.println("Adafruit VL53L0X Distance Sensor");

    if (!lox.begin()) {
        Serial.println("Failed to find VL53L0X sensor!");
        while (1) delay(10);
    }
    Serial.println("VL53L0X initialized");

    // Configure for high accuracy
    lox.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_ACCURACY);

    // Test: set simulated distance
    lox.setDistance(185);
}

void loop() {
    uint16_t range = lox.readRange();
    uint8_t status = lox.readRangeStatus();

    Serial.print("Distance: ");
    Serial.print(range);
    Serial.print(" mm  Status: ");
    Serial.println(status == 0 ? "Valid" : "Error");

    delay(100);
}
