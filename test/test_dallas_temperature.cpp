#include "DallasTemperature.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    // OneWire needs to provide devices via search
    OneWire ow(4);

    // Add a DS18B20 device
    uint8_t addr1[8] = {0x28, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
    uint8_t addr2[8] = {0x28, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};
    ow.addDevice(addr1);
    ow.addDevice(addr2);

    DallasTemperature sensors(&ow);
    sensors.begin();

    // Should find 2 devices
    assert(sensors.getDeviceCount() == 2);

    // Get address
    uint8_t retrieved[8];
    assert(sensors.getAddress(retrieved, 0));
    assert(retrieved[0] == 0x28);
    assert(!sensors.getAddress(retrieved, 5)); // out of range

    // Default temperature
    sensors.requestTemperatures();
    float t = sensors.getTempCByIndex(0);
    assert(t > 20.0f && t < 25.0f); // default 22.5

    // Set custom temperature
    sensors.setTemperatureByIndex(0, 100.0f);
    assert(fabsf(sensors.getTempCByIndex(0) - 100.0f) < 0.01f);

    // Fahrenheit conversion
    sensors.setTemperatureByIndex(1, 0.0f);
    assert(fabsf(sensors.getTempFByIndex(1) - 32.0f) < 0.01f);

    // By address
    sensors.setTemperature(addr1, -40.0f);
    assert(fabsf(sensors.getTempC(addr1) - (-40.0f)) < 0.01f);

    // Resolution
    sensors.setResolution(9);
    assert(sensors.getResolution() == 9);

    // Wait for conversion
    sensors.setWaitForConversion(false);
    assert(!sensors.getWaitForConversion());

    // Invalid index
    assert(sensors.getTempCByIndex(99) == -127.0f);

    printf("test_dallas_temperature: all assertions passed\n");
    return 0;
}
