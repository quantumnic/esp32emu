#include "SensirionI2CSen5x.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    SensirionI2CSen5x sen;
    sen.begin();
    assert(sen._test_isInitialized());

    // Not measuring yet
    float pm1, pm25, pm4, pm10, hum, temp, voc, nox;
    assert(sen.readMeasuredValues(pm1, pm25, pm4, pm10, hum, temp, voc, nox) != 0);

    // Start measurement
    assert(sen.startMeasurement() == 0);
    assert(sen._test_isMeasuring());

    // Read defaults
    assert(sen.readMeasuredValues(pm1, pm25, pm4, pm10, hum, temp, voc, nox) == 0);
    assert(fabsf(pm25 - 12.0f) < 0.01f);
    assert(fabsf(temp - 22.5f) < 0.01f);

    // Inject custom values
    sen._test_setValues(1.0f, 5.0f, 7.0f, 9.0f, 55.0f, 28.0f, 200.0f, 5.0f);
    assert(sen.readMeasuredValues(pm1, pm25, pm4, pm10, hum, temp, voc, nox) == 0);
    assert(fabsf(pm1 - 1.0f) < 0.01f);
    assert(fabsf(hum - 55.0f) < 0.01f);
    assert(fabsf(voc - 200.0f) < 0.01f);
    assert(fabsf(nox - 5.0f) < 0.01f);

    // Temperature offset
    assert(sen.setTemperatureOffsetSimple(2.5f) == 0);
    assert(fabsf(sen._test_getTempOffset() - 2.5f) < 0.01f);

    // Serial number
    unsigned char serial[32];
    assert(sen.getSerialNumber(serial, 32) == 0);
    assert(serial[0] == 'S');

    // Stop and reset
    assert(sen.stopMeasurement() == 0);
    assert(!sen._test_isMeasuring());
    assert(sen.deviceReset() == 0);

    printf("test_sen5x: all assertions passed\n");
    return 0;
}
