#include "Adafruit_BME680.h"
#include <cassert>
#include <cmath>
#include <cstdio>

int main() {
    Adafruit_BME680 bme;
    assert(!bme._isInitialized());
    assert(bme.begin());
    assert(bme._isInitialized());
    assert(bme._getAddr() == 0x77);

    // Alternate address
    Adafruit_BME680 bme2;
    assert(bme2.begin(0x76));
    assert(bme2._getAddr() == 0x76);

    // Default readings
    assert(bme.performReading());
    assert(std::fabs(bme.temperature - 22.5f) < 0.01f);
    assert(std::fabs(bme.humidity - 45.0f) < 0.01f);
    assert(std::fabs(bme.pressure - 101325.0f) < 1.0f);
    assert(std::fabs(bme.gas_resistance - 50000.0f) < 1.0f);

    // Set test values
    bme._setTemperature(35.0f);
    bme._setHumidity(90.0f);
    bme._setPressure(95000.0f);
    bme._setGasResistance(120000.0f);
    assert(bme.performReading());
    assert(std::fabs(bme.temperature - 35.0f) < 0.01f);
    assert(std::fabs(bme.humidity - 90.0f) < 0.01f);
    assert(std::fabs(bme.pressure - 95000.0f) < 1.0f);
    assert(std::fabs(bme.gas_resistance - 120000.0f) < 1.0f);

    // Oversampling config
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_4X);
    bme.setPressureOversampling(BME680_OS_2X);
    assert(bme._getTempOversampling() == BME680_OS_8X);
    assert(bme._getHumOversampling() == BME680_OS_4X);
    assert(bme._getPresOversampling() == BME680_OS_2X);

    // Gas heater
    bme.setGasHeater(300, 100);
    assert(bme._getHeaterTemp() == 300);
    assert(bme._getHeaterDuration() == 100);

    // IIR filter
    bme.setIIRFilterSize(BME680_FILTER_SIZE_15);

    // Altitude
    bme._setPressure(101325.0f);
    float alt = bme.readAltitude(1013.25f);
    assert(std::fabs(alt) < 1.0f);

    // Not initialized => performReading fails
    Adafruit_BME680 bme3;
    assert(!bme3.performReading());

    printf("test_bme680: all assertions passed\n");
    return 0;
}
