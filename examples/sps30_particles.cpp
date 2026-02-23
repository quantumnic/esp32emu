// SPS30 particulate matter sensor — mass and number concentrations
#include <Arduino.h>
#include "SPS30.h"

SPS30 sensor;

void setup() {
    Serial.begin(115200);
    if (!sensor.begin()) {
        Serial.println("SPS30 not found!");
        while (1) delay(100);
    }
    char serial[32];
    sensor.getSerialNumber(serial, 32);
    Serial.print("SPS30 serial: "); Serial.println(serial);
    sensor.start();
    Serial.println("Measuring particles...");
}

void loop() {
    sps30_measurement m;
    if (sensor.readMeasurement(&m)) {
        Serial.println("── Particle Data ──");
        Serial.print("PM1.0:  "); Serial.print(m.mc_1p0, 1); Serial.println(" µg/m³");
        Serial.print("PM2.5:  "); Serial.print(m.mc_2p5, 1); Serial.println(" µg/m³");
        Serial.print("PM4.0:  "); Serial.print(m.mc_4p0, 1); Serial.println(" µg/m³");
        Serial.print("PM10:   "); Serial.print(m.mc_10p0, 1); Serial.println(" µg/m³");
        Serial.print("Typical size: "); Serial.print(m.typical_particle_size, 2); Serial.println(" µm");
    }
    delay(1000);
}
