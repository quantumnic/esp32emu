#include "Adafruit_MCP9808.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_MCP9808 sensor;
    assert(sensor.begin(0x19));
    assert(sensor.isInitialized());
    assert(sensor.getAddress() == 0x19);

    // Default temp
    assert(fabsf(sensor.readTempC() - 25.0f) < 0.01f);
    assert(fabsf(sensor.readTempF() - 77.0f) < 0.01f);

    // Set temp
    sensor.setTemperature(100.0f);
    assert(fabsf(sensor.readTempC() - 100.0f) < 0.01f);
    assert(fabsf(sensor.readTempF() - 212.0f) < 0.01f);

    // Negative
    sensor.setTemperature(-40.0f);
    assert(fabsf(sensor.readTempC() - (-40.0f)) < 0.01f);

    // Resolution
    sensor.setResolution(0x01);
    assert(sensor.getResolution() == 0x01);

    // Shutdown
    assert(!sensor.isShutdown());
    sensor.shutdown();
    assert(sensor.isShutdown());
    sensor.wake();
    assert(!sensor.isShutdown());

    // Alerts
    sensor.setAlertUpper(50.0f);
    sensor.setAlertLower(5.0f);
    sensor.setAlertCritical(80.0f);
    assert(fabsf(sensor.getAlertUpper() - 50.0f) < 0.01f);
    assert(fabsf(sensor.getAlertLower() - 5.0f) < 0.01f);
    assert(fabsf(sensor.getAlertCritical() - 80.0f) < 0.01f);

    printf("test_mcp9808: all assertions passed\n");
    return 0;
}
