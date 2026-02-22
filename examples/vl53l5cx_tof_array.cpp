#include <Arduino.h>
#include <SparkFun_VL53L5CX.h>

SparkFun_VL53L5CX tof;

void setup() {
    Serial.begin(115200);
    Serial.println("VL53L5CX 8x8 ToF Array");

    if (!tof.begin()) {
        Serial.println("Failed to find VL53L5CX!");
        while (1) delay(10);
    }

    tof.setResolution(VL53L5CX_RESOLUTION_8X8);
    tof.setRangingFrequency(15);
    tof.startRanging();
    Serial.println("VL53L5CX initialized (8x8 mode)");
}

void loop() {
    if (tof.isDataReady()) {
        VL53L5CX_ResultsData results;
        tof.getRangingData(&results);

        Serial.println("Distance map (mm):");
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                int idx = row * 8 + col;
                Serial.printf("%5d", results.distance_mm[idx]);
            }
            Serial.println();
        }
        Serial.println("---");
    }
    delay(100);
}
