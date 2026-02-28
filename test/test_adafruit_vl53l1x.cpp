// esp32emu test — Adafruit_VL53L1X
#include <cassert>
#include <cstdio>
#define ESP32EMU_MOCK
#include "Arduino.h"
#include "Adafruit_VL53L1X.h"

int main() {
    // Test init
    {
        Adafruit_VL53L1X sensor;
        assert(!sensor.isBegun());
        assert(sensor.begin());
        assert(sensor.isBegun());
    }

    // Test distance
    {
        Adafruit_VL53L1X sensor;
        sensor.begin();
        assert(sensor.distance() == 150); // default
        sensor.setDistance(400);
        assert(sensor.distance() == 400);
    }

    // Test ranging
    {
        Adafruit_VL53L1X sensor;
        sensor.begin();
        assert(!sensor.isRanging());
        assert(sensor.startRanging());
        assert(sensor.isRanging());
        assert(sensor.stopRanging());
        assert(!sensor.isRanging());
    }

    // Test distance mode
    {
        Adafruit_VL53L1X sensor;
        sensor.begin();
        assert(sensor.getDistanceMode() == Adafruit_VL53L1X::LONG);
        assert(sensor.setDistanceMode(Adafruit_VL53L1X::SHORT));
        assert(sensor.getDistanceMode() == Adafruit_VL53L1X::SHORT);
    }

    // Test timing budget
    {
        Adafruit_VL53L1X sensor;
        sensor.begin();
        assert(sensor.setTimingBudget(100));
        assert(sensor.getTimingBudget() == 100);
        assert(!sensor.setTimingBudget(42)); // invalid value
        assert(sensor.getTimingBudget() == 100); // unchanged
    }

    // Test ROI
    {
        Adafruit_VL53L1X sensor;
        sensor.begin();
        assert(sensor.setROI(8, 8));
        uint8_t w, h;
        sensor.getROI(&w, &h);
        assert(w == 8 && h == 8);
        assert(!sensor.setROI(2, 2)); // too small
    }

    // Test signal/ambient/status
    {
        Adafruit_VL53L1X sensor;
        sensor.begin();
        sensor.setSignalRate(8000);
        sensor.setAmbientRate(100);
        sensor.setRangeStatus(1);
        assert(sensor.signalRate() == 8000);
        assert(sensor.ambientRate() == 100);
        assert(sensor.rangeStatus() == 1);
    }

    // Test offset/xtalk
    {
        Adafruit_VL53L1X sensor;
        sensor.begin();
        assert(sensor.setOffset(-20));
        assert(sensor.getOffset() == -20);
        assert(sensor.setXTalk(500));
        assert(sensor.getXTalk() == 500);
    }

    // Test with shutdown pin
    {
        Adafruit_VL53L1X sensor(4, 5);
        assert(sensor.begin(0x30));
        assert(sensor.isBegun());
    }

    printf("test_adafruit_vl53l1x: all assertions passed\n");
    return 0;
}
