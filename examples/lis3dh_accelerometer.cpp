// esp32emu example: LIS3DH Accelerometer
// Reads 3-axis acceleration data.

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_LIS3DH.h>

Adafruit_LIS3DH lis;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!lis.begin(0x18)) {
        Serial.println("LIS3DH not found!");
        while (1) delay(10);
    }

    lis.setRange(LIS3DH_RANGE_4_G);
    lis.setDataRate(LIS3DH_DATARATE_50_HZ);
    lis.setClick(0x15, 80, 10, 20, 255);

    // Test: simulate tilted orientation
    lis.setRawData(3000, -1500, 15000);

    Serial.println("LIS3DH Accelerometer Demo");
}

void loop() {
    sensors_event_t event;
    lis.getEvent(&event);
    Serial.print("X: "); Serial.print(event.acceleration.x, 2);
    Serial.print("  Y: "); Serial.print(event.acceleration.y, 2);
    Serial.print("  Z: "); Serial.println(event.acceleration.z, 2);

    uint8_t click = lis.getClick();
    if (click) Serial.println("Click detected!");

    delay(100);
}
