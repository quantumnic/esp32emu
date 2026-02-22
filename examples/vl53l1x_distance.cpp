// VL53L1X ToF distance sensor example
#include "Arduino.h"
#include "VL53L1X.h"

VL53L1X sensor;

void setup() {
    Serial.begin(115200);
    sensor.init();
    sensor.setDistanceMode(VL53L1X::Long);
    sensor.setMeasurementTimingBudget(50000);
    sensor.startContinuous(50);

    sensor.setDistance(832);  // Simulate 832mm distance
    Serial.println("VL53L1X ready");
}

void loop() {
    uint16_t dist = sensor.read();
    printf("Distance: %d mm\n", dist);

    if (dist < 200) printf("  -> Very close!\n");
    else if (dist < 1000) printf("  -> Near range\n");
    else printf("  -> Far range\n");

    delay(100);
}
