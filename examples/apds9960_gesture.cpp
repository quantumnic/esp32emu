// APDS-9960 gesture sensor example
// Detects hand gestures (up/down/left/right/near/far) and reads proximity + color

#include <Arduino.h>
#include "APDS9960.h"

APDS9960 sensor;

void setup() {
    Serial.begin(115200);
    Serial.println("APDS-9960 Gesture Sensor");

    if (!sensor.init()) {
        Serial.println("ERROR: APDS-9960 init failed!");
        return;
    }
    sensor.enableGestureSensor(true);
    sensor.enableProximitySensor();
    sensor.enableLightSensor();
    Serial.println("Sensor ready. Wave your hand!");

    // Inject test data for demo
    sensor.setGesture(APDS9960::DIR_LEFT);
    sensor.setProximity(120);
    sensor.setColor(350, 420, 280);
    sensor.setAmbientLight(800);
}

void loop() {
    if (sensor.isGestureAvailable()) {
        int gesture = sensor.readGesture();
        switch (gesture) {
            case APDS9960::DIR_UP:    Serial.println("Gesture: UP"); break;
            case APDS9960::DIR_DOWN:  Serial.println("Gesture: DOWN"); break;
            case APDS9960::DIR_LEFT:  Serial.println("Gesture: LEFT"); break;
            case APDS9960::DIR_RIGHT: Serial.println("Gesture: RIGHT"); break;
            case APDS9960::DIR_NEAR:  Serial.println("Gesture: NEAR"); break;
            case APDS9960::DIR_FAR:   Serial.println("Gesture: FAR"); break;
        }
    }

    uint8_t proximity;
    sensor.readProximity(proximity);
    Serial.print("Proximity: "); Serial.println(proximity);

    uint16_t r, g, b, amb;
    sensor.readRedLight(r);
    sensor.readGreenLight(g);
    sensor.readBlueLight(b);
    sensor.readAmbientLight(amb);
    Serial.print("RGB: "); Serial.print(r);
    Serial.print(", "); Serial.print(g);
    Serial.print(", "); Serial.println(b);
    Serial.print("Ambient: "); Serial.println(amb);

    delay(500);
}
