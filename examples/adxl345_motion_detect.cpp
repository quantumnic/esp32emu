// ADXL345 accelerometer motion detection example
// Reads acceleration, calculates tilt, detects movement

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_ADXL345_U.h>
#include <math.h>

Adafruit_ADXL345_Unified accel(54321);

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!accel.begin()) {
        Serial.println("Failed to find ADXL345!");
        while (1) delay(10);
    }
    Serial.println("ADXL345 Motion Detector");
    Serial.println("========================");

    accel.setRange(ADXL345_RANGE_4_G);
    accel.setDataRate(ADXL345_DATARATE_100_HZ);

    // Configure tap detection
    accel.setTapThreshold(80);
    accel.setTapDuration(10);
    accel.setDoubleTapLatency(50);
    accel.setDoubleTapWindow(200);

    // Configure free-fall detection
    accel.setFreeFallThreshold(7);
    accel.setFreeFallDuration(30);

    // Inject test values: slightly tilted device
    accel.test_setAccel(0.8f, -0.3f, 9.7f);
}

void loop() {
    sensors_event_t_adxl event;
    accel.getEvent(&event);

    float x = event.acceleration.x;
    float y = event.acceleration.y;
    float z = event.acceleration.z;

    // Calculate tilt angles
    float pitch = atan2(x, sqrt(y * y + z * z)) * 180.0f / M_PI;
    float roll  = atan2(y, sqrt(x * x + z * z)) * 180.0f / M_PI;

    // Total acceleration magnitude
    float totalG = sqrt(x * x + y * y + z * z) / 9.81f;

    Serial.print("Accel X: "); Serial.print(x, 2);
    Serial.print(" Y: "); Serial.print(y, 2);
    Serial.print(" Z: "); Serial.print(z, 2);
    Serial.print(" m/s²  |  ");
    Serial.print("Pitch: "); Serial.print(pitch, 1);
    Serial.print("° Roll: "); Serial.print(roll, 1);
    Serial.print("°  |  "); Serial.print(totalG, 2);
    Serial.println(" G");

    // Orientation detection
    if (fabs(z) > 8.0f && fabs(x) < 2.0f && fabs(y) < 2.0f) {
        Serial.println(z > 0 ? "  📱 Flat (face up)" : "  📱 Flat (face down)");
    } else if (fabs(x) > 8.0f) {
        Serial.println(x > 0 ? "  📱 Tilted right" : "  📱 Tilted left");
    } else if (fabs(y) > 8.0f) {
        Serial.println(y > 0 ? "  📱 Portrait" : "  📱 Portrait (inverted)");
    }

    // Motion alert
    if (totalG > 1.5f) {
        Serial.println("  ⚠️  Strong motion detected!");
    } else if (totalG < 0.5f) {
        Serial.println("  🪂 Free fall detected!");
    }

    delay(200);
}
