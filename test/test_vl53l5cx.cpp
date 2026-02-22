#include "SparkFun_VL53L5CX.h"
#include <cassert>
#include <cstdio>

int main() {
    // Test 1: Initialization
    {
        SparkFun_VL53L5CX sensor;
        assert(sensor.begin());
    }

    // Test 2: Resolution
    {
        SparkFun_VL53L5CX sensor;
        sensor.begin();
        assert(sensor.getResolution() == VL53L5CX_RESOLUTION_4X4);
        assert(sensor.setResolution(VL53L5CX_RESOLUTION_8X8));
        assert(sensor.getResolution() == VL53L5CX_RESOLUTION_8X8);
        assert(!sensor.setResolution(99));  // invalid
    }

    // Test 3: Ranging config
    {
        SparkFun_VL53L5CX sensor;
        sensor.begin();
        sensor.setRangingFrequency(30);
        assert(sensor.getRangingFrequency() == 30);
        sensor.setIntegrationTime(10);
        assert(sensor.getIntegrationTime() == 10);
        sensor.setSharpenerPercent(15);
        assert(sensor.getSharpenerPercent() == 15);
    }

    // Test 4: Start/stop ranging
    {
        SparkFun_VL53L5CX sensor;
        sensor.begin();
        assert(sensor.startRanging());
        assert(sensor.stopRanging());
    }

    // Test 5: Read data
    {
        SparkFun_VL53L5CX sensor;
        sensor.begin();
        assert(sensor.isDataReady());
        VL53L5CX_ResultsData results;
        assert(sensor.getRangingData(&results));
        assert(results.distance_mm[0] == 500);
        assert(results.target_status[0] == VL53L5CX_STATUS_VALID);
        // After read, data_ready goes false
        assert(!sensor.isDataReady());
    }

    // Test 6: Test helpers
    {
        SparkFun_VL53L5CX sensor;
        sensor.begin();
        sensor._test_set_distance(0, 1234);
        sensor._test_set_data_ready(true);
        VL53L5CX_ResultsData results;
        sensor.getRangingData(&results);
        assert(results.distance_mm[0] == 1234);
    }

    printf("test_vl53l5cx: all assertions passed\n");
    return 0;
}
