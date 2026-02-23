#include "Arduino.h"
#include "SDS011.h"
#include <cassert>
#include <cmath>
#include <cstdio>

int main() {
    SDS011 sds;
    HardwareSerial Serial2;
    sds.begin(Serial2);
    assert(sds.test_isStarted());

    // Default read
    float pm25 = 0, pm10 = 0;
    int err = sds.read(&pm25, &pm10);
    assert(err == 0);
    assert(fabs(pm25 - 12.5f) < 0.01f);
    assert(fabs(pm10 - 25.0f) < 0.01f);

    // Set values
    sds.test_setPM25(35.2f);
    sds.test_setPM10(48.7f);
    err = sds.read(&pm25, &pm10);
    assert(err == 0);
    assert(fabs(pm25 - 35.2f) < 0.01f);
    assert(fabs(pm10 - 48.7f) < 0.01f);

    // Sleep/wake
    assert(!sds.test_isSleeping());
    sds.sleep();
    assert(sds.test_isSleeping());
    sds.wakeup();
    assert(!sds.test_isSleeping());

    // Working period
    sds.setWorkingPeriod(5);
    assert(sds.getWorkingPeriod() == 5);

    // Query mode
    assert(!sds.isQueryMode());
    sds.setQueryMode();
    assert(sds.isQueryMode());
    sds.setActiveMode();
    assert(!sds.isQueryMode());

    // Error injection
    sds.test_setErrorCode(1);
    err = sds.read(&pm25, &pm10);
    assert(err == 1);

    // Firmware
    sds.test_setFirmware("20.03");
    assert(sds.firmwareVersion() == "20.03");

    // Begin with pins
    SDS011 sds2;
    sds2.begin(16, 17);
    assert(sds2.test_isStarted());

    printf("test_sds011: all assertions passed\n");
    return 0;
}
