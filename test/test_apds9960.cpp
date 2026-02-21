#include "APDS9960.h"
#include <cassert>
#include <cstdio>

int main() {
    APDS9960 sensor;

    // Init
    assert(sensor.init());
    assert(sensor.isInitialized());

    // Gesture
    assert(!sensor.isGestureAvailable());
    sensor.setGesture(APDS9960::DIR_LEFT);
    assert(sensor.isGestureAvailable());
    assert(sensor.readGesture() == APDS9960::DIR_LEFT);
    assert(!sensor.isGestureAvailable()); // consumed

    // Proximity
    sensor.enableProximitySensor();
    sensor.setProximity(150);
    uint8_t prox = 0;
    assert(sensor.readProximity(prox));
    assert(prox == 150);

    // Ambient light
    sensor.enableLightSensor();
    sensor.setAmbientLight(1024);
    uint16_t amb = 0;
    assert(sensor.readAmbientLight(amb));
    assert(amb == 1024);

    // Color
    sensor.setColor(300, 400, 500);
    uint16_t r, g, b;
    assert(sensor.readRedLight(r));
    assert(sensor.readGreenLight(g));
    assert(sensor.readBlueLight(b));
    assert(r == 300 && g == 400 && b == 500);

    // Gesture directions
    sensor.setGesture(APDS9960::DIR_UP);
    assert(sensor.readGesture() == 1);
    sensor.setGesture(APDS9960::DIR_FAR);
    assert(sensor.readGesture() == 6);

    printf("test_apds9960: all assertions passed\n");
    return 0;
}
