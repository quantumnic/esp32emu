// esp32emu test — Adafruit_VL53L0X
#include <cassert>
#include <cstdio>
#define ESP32EMU_MOCK
#include "Arduino.h"
#include "Adafruit_VL53L0X.h"

int main() {
    // Test init
    {
        Adafruit_VL53L0X sensor;
        assert(!sensor.isBegun());
        assert(sensor.begin());
        assert(sensor.isBegun());
    }

    // Test custom address
    {
        Adafruit_VL53L0X sensor;
        assert(sensor.begin(0x30));
        assert(sensor.isBegun());
    }

    // Test readRange + setDistance
    {
        Adafruit_VL53L0X sensor;
        sensor.begin();
        assert(sensor.readRange() == 100); // default
        sensor.setDistance(250);
        assert(sensor.readRange() == 250);
    }

    // Test range status
    {
        Adafruit_VL53L0X sensor;
        sensor.begin();
        assert(sensor.readRangeStatus() == 0);
        sensor.setRangeStatus(2);
        assert(sensor.readRangeStatus() == 2);
    }

    // Test continuous mode
    {
        Adafruit_VL53L0X sensor;
        sensor.begin();
        assert(!sensor.isContinuous());
        sensor.startRangeContinuous(100);
        assert(sensor.isContinuous());
        assert(sensor.isRangeComplete());
        sensor.setDistance(300);
        assert(sensor.readRangeResult() == 300);
        sensor.stopRangeContinuous();
        assert(!sensor.isContinuous());
    }

    // Test sense config
    {
        Adafruit_VL53L0X sensor;
        sensor.begin(0x29, false, nullptr, Adafruit_VL53L0X::VL53L0X_SENSE_LONG_RANGE);
        assert(sensor.getSenseConfig() == Adafruit_VL53L0X::VL53L0X_SENSE_LONG_RANGE);
        assert(sensor.configSensor(Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_ACCURACY));
        assert(sensor.getSenseConfig() == Adafruit_VL53L0X::VL53L0X_SENSE_HIGH_ACCURACY);
    }

    // Test timeout
    {
        Adafruit_VL53L0X sensor;
        sensor.begin();
        sensor.setTimeout(1000);
        assert(sensor.getTimeout() == 1000);
        assert(!sensor.timeoutOccurred());
    }

    // Test address change
    {
        Adafruit_VL53L0X sensor;
        sensor.begin();
        assert(sensor.setAddress(0x35));
    }

    printf("test_adafruit_vl53l0x: all assertions passed\n");
    return 0;
}
