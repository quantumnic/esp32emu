#include "Arduino.h"
#include "MHZ19.h"
#include <cassert>
#include <cstdio>

int main() {
    MHZ19 mhz;
    HardwareSerial Serial2;
    mhz.begin(Serial2);
    assert(mhz.test_isStarted());

    // Default CO2
    assert(mhz.getCO2() == 400);
    assert(mhz.getTemperature() == 24.0f);

    // Set values
    mhz.test_setCO2(850);
    mhz.test_setTemperature(22.5f);
    assert(mhz.getCO2() == 850);
    assert(mhz.getTemperature() == 22.5f);

    // Auto calibration
    assert(mhz.getABC() == true);
    mhz.autoCalibration(false);
    assert(mhz.getABC() == false);
    assert(mhz.test_getAutoCalibration() == false);

    // Range
    mhz.setRange(10000);
    assert(mhz.getRange() == 10000);

    // Calibration
    mhz.calibrateZero();
    assert(mhz.test_wasZeroCalibrated());
    mhz.calibrateSpan(5000);
    assert(mhz.test_getSpan() == 5000);

    // Min/max CO2
    mhz.test_setMinCO2(350);
    mhz.test_setMaxCO2(10000);
    assert(mhz.getMinCO2() == 350);
    assert(mhz.getMaxCO2() == 10000);

    // Accuracy
    mhz.test_setAccuracy(95);
    assert(mhz.getAccuracy() == 95);

    // Recovery reset
    mhz.recoveryReset();
    assert(mhz.test_wasRecoveryReset());

    printf("test_mhz19: all assertions passed\n");
    return 0;
}
