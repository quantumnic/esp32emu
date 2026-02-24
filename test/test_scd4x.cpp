#include "Arduino.h"
#include "Wire.h"
#include "SensirionI2CScd4x.h"
#include <cassert>
#include <cstdio>

int main() {
    SensirionI2CScd4x scd;
    TwoWire Wire;
    scd.begin(Wire);
    assert(scd.test_isStarted());

    // Start measurement
    assert(scd.startPeriodicMeasurement() == 0);
    assert(scd.test_isMeasuring());

    // Data not ready yet
    bool ready = false;
    scd.getDataReadyFlag(ready);
    assert(!ready);

    // Set test data
    scd.test_setCO2(823);
    scd.test_setTemperature(22.3f);
    scd.test_setHumidity(45.5f);
    scd.test_setDataReady(true);

    scd.getDataReadyFlag(ready);
    assert(ready);

    uint16_t co2;
    float temp, hum;
    assert(scd.readMeasurement(co2, temp, hum) == 0);
    assert(co2 == 823);
    assert(temp == 22.3f);
    assert(hum == 45.5f);

    // Temperature offset
    scd.setTemperatureOffset(2.5f);
    float offset;
    scd.getTemperatureOffset(offset);
    assert(offset == 2.5f);

    // Altitude compensation
    scd.setSensorAltitude(500);
    uint16_t alt;
    scd.getSensorAltitude(alt);
    assert(alt == 500);

    // Ambient pressure
    scd.setAmbientPressure(1013);
    assert(scd.test_getAmbientPressure() == 1013);

    // Auto self-calibration
    scd.setAutomaticSelfCalibration(0);
    uint16_t asc;
    scd.getAutomaticSelfCalibration(asc);
    assert(asc == 0);

    // Forced recalibration
    uint16_t frc;
    scd.performForcedRecalibration(420, frc);
    assert(scd.test_wasForcedRecal());
    assert(scd.test_getFrcTarget() == 420);

    // Serial number
    uint16_t s0, s1, s2;
    scd.getSerialNumber(s0, s1, s2);
    assert(s0 == 0xBEEF);

    // Persist settings
    scd.persistSettings();
    assert(scd.test_wasPersisted());

    // Power management
    scd.powerDown();
    assert(scd.test_isPoweredDown());
    scd.wakeUp();
    assert(!scd.test_isPoweredDown());

    // Low power mode
    scd.stopPeriodicMeasurement();
    scd.startLowPowerPeriodicMeasurement();
    assert(scd.test_isLowPower());

    // Single shot
    scd.measureSingleShot();

    // Factory reset
    scd.performFactoryReset();

    printf("test_scd4x: all assertions passed\n");
    return 0;
}
