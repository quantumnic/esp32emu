// esp32emu test — Adafruit BME680 gas sensor
#include "Adafruit_BME680.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_BME680 bme;

    // Init
    assert(!bme._isInitialized());
    assert(bme.begin(0x76));
    assert(bme._isInitialized());
    assert(bme._getAddr() == 0x76);

    // Oversampling
    bme.setTemperatureOversampling(BME680_OS_8X);
    bme.setHumidityOversampling(BME680_OS_2X);
    bme.setPressureOversampling(BME680_OS_4X);
    assert(bme._getTempOversampling() == BME680_OS_8X);
    assert(bme._getHumOversampling() == BME680_OS_2X);
    assert(bme._getPresOversampling() == BME680_OS_4X);

    // Filter and heater
    bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme.setGasHeater(300, 100);
    assert(bme._getHeaterTemp() == 300);
    assert(bme._getHeaterDuration() == 100);

    // Reading defaults
    assert(bme.performReading());
    assert(fabs(bme.temperature - 22.5f) < 0.01f);
    assert(fabs(bme.humidity - 45.0f) < 0.01f);
    assert(fabs(bme.pressure - 101325.0f) < 1.0f);
    assert(fabs(bme.gas_resistance - 50000.0f) < 1.0f);

    // Inject values
    bme._setTemperature(30.0f);
    bme._setHumidity(80.0f);
    bme._setPressure(95000.0f);
    bme._setGasResistance(100000.0f);
    assert(bme.performReading());
    assert(fabs(bme.temperature - 30.0f) < 0.01f);
    assert(fabs(bme.humidity - 80.0f) < 0.01f);
    assert(fabs(bme.gas_resistance - 100000.0f) < 1.0f);

    // Altitude
    float alt = bme.readAltitude(1013.25f);
    assert(alt > 0); // lower pressure = positive altitude

    printf("test_bme680: all assertions passed\n");
    return 0;
}
