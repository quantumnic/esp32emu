// BNO08x rotation vector example — reads quaternion orientation
#include <Arduino.h>
#include "BNO08x.h"

BNO08x bno;

void setup() {
    Serial.begin(115200);
    if (!bno.begin_I2C()) {
        Serial.println("BNO08x not found!");
        while (1) delay(100);
    }
    bno.enableReport(SH2_ROTATION_VECTOR, 5000);
    bno.enableReport(SH2_ACCELEROMETER, 10000);
    Serial.println("BNO08x ready — reading orientation");
}

void loop() {
    sh2_SensorValue_t event;

    if (bno.wasReset()) {
        Serial.println("BNO08x was reset, re-enabling reports");
        bno.enableReport(SH2_ROTATION_VECTOR, 5000);
        bno.enableReport(SH2_ACCELEROMETER, 10000);
    }

    if (bno.getSensorEvent(&event)) {
        switch (event.sensorId) {
            case SH2_ROTATION_VECTOR:
                Serial.print("Rotation: i=");
                Serial.print(event.un_rotationVector.i, 3);
                Serial.print(" j=");
                Serial.print(event.un_rotationVector.j, 3);
                Serial.print(" k=");
                Serial.print(event.un_rotationVector.k, 3);
                Serial.print(" real=");
                Serial.println(event.un_rotationVector.real, 3);
                break;
            case SH2_ACCELEROMETER:
                Serial.print("Accel: x=");
                Serial.print(event.un_accelerometer.x, 2);
                Serial.print(" y=");
                Serial.print(event.un_accelerometer.y, 2);
                Serial.print(" z=");
                Serial.println(event.un_accelerometer.z, 2);
                break;
        }
    }
    delay(10);
}
