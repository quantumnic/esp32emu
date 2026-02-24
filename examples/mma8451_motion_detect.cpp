#include <Arduino.h>
#include <Adafruit_MMA8451.h>

Adafruit_MMA8451 accel;

void setup() {
    Serial.begin(115200);
    Serial.println("MMA8451 Motion Detector");

    if (!accel.begin()) {
        Serial.println("MMA8451 not found!");
        while (1) delay(10);
    }

    accel.setRange(MMA8451_RANGE_2_G);
    accel.setDataRate(MMA8451_DATARATE_50_HZ);

    Serial.println("Range: ±2G");
    Serial.println("Ready.");
}

void loop() {
    accel.read();

    Serial.print("X: ");
    Serial.print(accel.x_g, 2);
    Serial.print("g  Y: ");
    Serial.print(accel.y_g, 2);
    Serial.print("g  Z: ");
    Serial.print(accel.z_g, 2);
    Serial.print("g  ");

    uint8_t orient = accel.getOrientation();
    switch (orient) {
        case 0: Serial.print("[Portrait Up]"); break;
        case 1: Serial.print("[Portrait Down]"); break;
        case 2: Serial.print("[Landscape Right]"); break;
        case 3: Serial.print("[Landscape Left]"); break;
    }
    Serial.println();

    delay(200);
}
