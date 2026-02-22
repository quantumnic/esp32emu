// LSM6DSO 6-axis IMU motion tracking
#include <Wire.h>
#include <Adafruit_LSM6DS.h>

Adafruit_LSM6DSO lsm;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!lsm.begin_I2C()) {
        Serial.println("LSM6DSO not found!");
        while (1) delay(1000);
    }

    lsm.setAccelRange(LSM6DS_ACCEL_RANGE_4_G);
    lsm.setGyroRange(LSM6DS_GYRO_RANGE_500_DPS);
    lsm.setAccelDataRate(LSM6DS_RATE_104_HZ);
    Serial.println("LSM6DSO ready");
}

void loop() {
    sensors_event_t accel, gyro, temp;
    lsm.getEvent(&accel, &gyro, &temp);

    Serial.print("Accel X="); Serial.print(accel.acceleration.x, 2);
    Serial.print(" Y="); Serial.print(accel.acceleration.y, 2);
    Serial.print(" Z="); Serial.print(accel.acceleration.z, 2);
    Serial.print("  Gyro X="); Serial.print(gyro.gyro.x, 2);
    Serial.print(" Y="); Serial.print(gyro.gyro.y, 2);
    Serial.print(" Z="); Serial.print(gyro.gyro.z, 2);
    Serial.print("  Temp="); Serial.print(temp.temperature, 1);
    Serial.println(" °C");

    delay(100);
}
