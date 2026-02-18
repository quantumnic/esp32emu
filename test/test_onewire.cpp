#include "OneWire.h"
#include "DallasTemperature.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    // Test 1: OneWire device search
    {
        OneWire ow(4);
        uint8_t addr1[8] = {0x28, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
        uint8_t addr2[8] = {0x28, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};
        ow.addDevice(addr1);
        ow.addDevice(addr2);

        assert(ow.reset() == 1);

        uint8_t found[8];
        assert(ow.search(found));
        assert(found[0] == 0x28);
        assert(found[1] == 0x01);
        assert(ow.search(found));
        assert(found[1] == 0x11);
        assert(!ow.search(found)); // no more

        ow.reset_search();
        assert(ow.search(found)); // can search again
    }

    // Test 2: OneWire read injection
    {
        OneWire ow(4);
        ow.injectReadData({0xAA, 0xBB, 0xCC});
        assert(ow.read() == 0xAA);
        assert(ow.read() == 0xBB);
        assert(ow.read() == 0xCC);
        assert(ow.read() == 0xFF); // empty returns 0xFF
    }

    // Test 3: DallasTemperature basic
    {
        OneWire ow(4);
        uint8_t addr1[8] = {0x28, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
        uint8_t addr2[8] = {0x28, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};
        ow.addDevice(addr1);
        ow.addDevice(addr2);

        DallasTemperature sensors(&ow);
        sensors.begin();
        assert(sensors.getDeviceCount() == 2);

        // Default temperature
        sensors.requestTemperatures();
        assert(std::fabs(sensors.getTempCByIndex(0) - 22.5f) < 0.01f);

        // Set custom temperature
        sensors.setTemperatureByIndex(0, 25.5f);
        assert(std::fabs(sensors.getTempCByIndex(0) - 25.5f) < 0.01f);

        // Fahrenheit conversion
        sensors.setTemperatureByIndex(1, 100.0f);
        assert(std::fabs(sensors.getTempFByIndex(1) - 212.0f) < 0.01f);

        // By address
        sensors.setTemperature(addr1, -10.0f);
        assert(std::fabs(sensors.getTempC(addr1) - (-10.0f)) < 0.01f);

        // Out of range index
        assert(std::fabs(sensors.getTempCByIndex(99) - (-127.0f)) < 0.01f);
    }

    // Test 4: Non-DS18B20 devices are ignored
    {
        OneWire ow(4);
        uint8_t addr_other[8] = {0x10, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}; // DS18S20
        uint8_t addr_ds18b20[8] = {0x28, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00};
        ow.addDevice(addr_other);
        ow.addDevice(addr_ds18b20);

        DallasTemperature sensors(&ow);
        sensors.begin();
        assert(sensors.getDeviceCount() == 1); // only DS18B20
    }

    // Test 5: Resolution
    {
        DallasTemperature sensors;
        sensors.setResolution(9);
        assert(sensors.getResolution() == 9);
    }

    printf("test_onewire: all assertions passed\n");
    return 0;
}
