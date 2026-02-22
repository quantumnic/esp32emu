#include "VL53L1X.h"
#include <cassert>
#include <cstdio>

int main() {
    VL53L1X sensor;
    assert(!sensor.isBegun());

    assert(sensor.init());
    assert(sensor.isBegun());

    // Distance mode
    sensor.setDistanceMode(VL53L1X::Short);
    assert(sensor.getDistanceMode() == VL53L1X::Short);
    sensor.setDistanceMode(VL53L1X::Long);
    assert(sensor.getDistanceMode() == VL53L1X::Long);

    // Timing budget
    sensor.setMeasurementTimingBudget(100000);
    assert(sensor.getMeasurementTimingBudget() == 100000);

    // Distance
    sensor.setDistance(1500);
    assert(sensor.read() == 1500);
    assert(sensor.readRangeContinuousMillimeters() == 1500);
    assert(sensor.readRangeSingleMillimeters() == 1500);

    // Continuous mode
    sensor.startContinuous(50);
    assert(sensor.isContinuous());
    sensor.stopContinuous();
    assert(!sensor.isContinuous());

    // Range status
    sensor.setRangeStatus(VL53L1X::SignalFail);
    assert(sensor.ranging_data_range_status() == VL53L1X::SignalFail);

    // Signal/ambient rate
    sensor.setSignalRate(1234);
    sensor.setAmbientRate(56);
    assert(sensor.ranging_data_peak_signal_count_rate_MCPS() == 1234);
    assert(sensor.ranging_data_ambient_count_rate_MCPS() == 56);

    // Address
    sensor.setAddress(0x30);
    assert(sensor.getAddress() == 0x30);

    assert(!sensor.timeoutOccurred());
    assert(sensor.dataReady());

    printf("test_vl53l1x: all assertions passed\n");
    return 0;
}
