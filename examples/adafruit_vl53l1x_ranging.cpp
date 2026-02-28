// esp32emu example — Adafruit VL53L1X Long-Range ToF Sensor
#include <Arduino.h>
#include <Adafruit_VL53L1X.h>

#define SHUTDOWN_PIN 4
#define IRQ_PIN      5

Adafruit_VL53L1X vl53(SHUTDOWN_PIN, IRQ_PIN);

void setup() {
    Serial.begin(115200);
    Serial.println("Adafruit VL53L1X Ranging Example");

    if (!vl53.begin(0x29)) {
        Serial.println("Failed to init VL53L1X!");
        while (1) delay(10);
    }
    Serial.println("VL53L1X initialized");

    // Long range mode, 50ms timing budget
    vl53.setDistanceMode(Adafruit_VL53L1X::LONG);
    vl53.setTimingBudget(50);

    // Set ROI to center 8x8
    vl53.setROI(8, 8);

    vl53.startRanging();

    // Test: set simulated distance
    vl53.setDistance(320);
}

void loop() {
    if (vl53.dataReady()) {
        int16_t dist = vl53.distance();
        vl53.clearInterrupt();

        Serial.print("Distance: ");
        Serial.print(dist);
        Serial.print(" mm  Signal: ");
        Serial.print(vl53.signalRate());
        Serial.print("  Status: ");
        Serial.println(vl53.rangeStatus());
    }

    delay(50);
}
