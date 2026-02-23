#include "SPS30.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    SPS30 sensor;

    assert(sensor.begin());
    assert(sensor._test_isInitialized());

    // Read fails before start
    sps30_measurement m;
    assert(!sensor.readMeasurement(&m));

    // Start measurement
    assert(sensor.start());
    assert(sensor._test_isMeasuring());

    // Read defaults
    assert(sensor.readMeasurement(&m));
    assert(fabsf(m.mc_2p5 - 12.0f) < 0.01f);
    assert(fabsf(m.typical_particle_size - 1.5f) < 0.01f);

    // Inject custom
    sps30_measurement custom{1.0f, 3.0f, 5.0f, 8.0f, 100.0f, 60.0f, 20.0f, 10.0f, 4.0f, 0.8f};
    sensor._test_setMeasurement(custom);
    assert(sensor.readMeasurement(&m));
    assert(fabsf(m.mc_1p0 - 1.0f) < 0.01f);
    assert(fabsf(m.nc_0p5 - 100.0f) < 0.01f);

    // Serial number
    char serial[32];
    assert(sensor.getSerialNumber(serial, 32));
    assert(serial[0] == 'S');

    // Sleep/wake
    assert(sensor.sleep());
    assert(sensor._test_isSleeping());
    assert(sensor.wakeup());
    assert(!sensor._test_isSleeping());

    // Stop
    assert(sensor.stop());
    assert(!sensor._test_isMeasuring());

    // Reset
    assert(sensor.reset());

    printf("test_sps30: all assertions passed\n");
    return 0;
}
