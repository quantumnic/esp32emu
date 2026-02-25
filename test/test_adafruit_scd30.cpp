// esp32emu — test_adafruit_scd30: SCD30 CO2/temperature/humidity sensor mock test
#include "Adafruit_SCD30.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_SCD30 scd;

    // Not initialized
    assert(!scd.esp32emu_is_initialized());
    assert(scd.begin(0x61));
    assert(scd.esp32emu_is_initialized());

    // Default values
    assert(scd.dataReady());
    assert(scd.read());
    assert(fabs(scd.CO2 - 415.0f) < 0.1f);
    assert(fabs(scd.temperature - 22.5f) < 0.01f);
    assert(fabs(scd.relative_humidity - 50.0f) < 0.01f);

    // After read, data not ready until new injection
    assert(!scd.dataReady());
    assert(!scd.read());

    // Inject CO2 value
    scd.esp32emu_set_co2(1200.0f);
    assert(scd.dataReady());
    assert(scd.read());
    assert(fabs(scd.CO2 - 1200.0f) < 0.1f);

    // Inject temperature and humidity
    scd.esp32emu_set_temperature(28.0f);
    scd.esp32emu_set_humidity(65.0f);
    assert(scd.read());
    assert(fabs(scd.temperature - 28.0f) < 0.01f);
    assert(fabs(scd.relative_humidity - 65.0f) < 0.01f);

    // Measurement interval
    scd.setMeasurementInterval(5);
    assert(scd.getMeasurementInterval() == 5);

    // Altitude offset
    scd.setAltitudeOffset(500);
    assert(scd.getAltitudeOffset() == 500);

    // Temperature offset
    scd.setTemperatureOffset(2.5f);
    assert(fabs(scd.getTemperatureOffset() - 2.5f) < 0.01f);

    // Forced recalibration
    scd.setForcedRecalibrationReference(450);
    assert(scd.getForcedRecalibrationReference() == 450);

    // Auto self-calibration
    scd.selfCalibrationEnabled(false);
    assert(!scd.selfCalibrationEnabled());
    scd.selfCalibrationEnabled(true);
    assert(scd.selfCalibrationEnabled());

    // Continuous measurement
    assert(!scd.esp32emu_is_running());
    scd.startContinuousMeasurement(1013);
    assert(scd.esp32emu_is_running());
    scd.stopContinuousMeasurement();
    assert(!scd.esp32emu_is_running());

    // Reset
    scd.reset();

    printf("test_adafruit_scd30: all assertions passed\n");
    return 0;
}
