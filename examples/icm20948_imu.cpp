// ICM-20948 9-axis IMU — accel, gyro, magnetometer
#include <Arduino.h>
#include "ICM20948.h"

ICM_20948_I2C imu;

void setup() {
    Serial.begin(115200);
    if (!imu.begin()) {
        Serial.println("ICM-20948 not found!");
        while (1) delay(100);
    }
    imu.setFullScale(ICM_20948_I2C::gpm4);
    imu.setFullScale(ICM_20948_I2C::dps500);
    Serial.println("ICM-20948 ready");
}

void loop() {
    if (imu.dataReady()) {
        imu.getAGMT();
        Serial.print("Accel: ");
        Serial.print(imu.accX(), 2); Serial.print(", ");
        Serial.print(imu.accY(), 2); Serial.print(", ");
        Serial.println(imu.accZ(), 2);
        Serial.print("Gyro:  ");
        Serial.print(imu.gyrX(), 2); Serial.print(", ");
        Serial.print(imu.gyrY(), 2); Serial.print(", ");
        Serial.println(imu.gyrZ(), 2);
        Serial.print("Mag:   ");
        Serial.print(imu.magX(), 1); Serial.print(", ");
        Serial.print(imu.magY(), 1); Serial.print(", ");
        Serial.println(imu.magZ(), 1);
        Serial.print("Temp:  ");
        Serial.println(imu.temp(), 1);
        Serial.println();
    }
    delay(100);
}
