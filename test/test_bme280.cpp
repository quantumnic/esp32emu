#include "Adafruit_BME280.h"
#include <cassert>
#include <cmath>
#include <cstdio>

int main() {
    // Basic init
    Adafruit_BME280 bme;
    assert(!bme.isInitialized());
    assert(bme.begin());
    assert(bme.isInitialized());
    assert(bme.getAddress() == 0x76);
    assert(bme.sensorID() == 0x60);

    // Alternate address
    Adafruit_BME280 bme2;
    assert(bme2.begin(BME280_ADDRESS_ALTERNATE));
    assert(bme2.getAddress() == 0x77);

    // Default readings
    assert(std::fabs(bme.readTemperature() - 22.5f) < 0.01f);
    assert(std::fabs(bme.readPressure() - 101325.0f) < 1.0f);
    assert(std::fabs(bme.readHumidity() - 45.0f) < 0.01f);

    // Set and read
    bme.setTemperature(30.0f);
    bme.setPressure(100000.0f);
    bme.setHumidity(80.0f);
    assert(std::fabs(bme.readTemperature() - 30.0f) < 0.01f);
    assert(std::fabs(bme.readPressure() - 100000.0f) < 1.0f);
    assert(std::fabs(bme.readHumidity() - 80.0f) < 0.01f);

    // Altitude calculation
    bme.setPressure(101325.0f);
    float alt = bme.readAltitude(101325.0f);
    assert(std::fabs(alt) < 1.0f); // at sea level = ~0m

    bme.setPressure(90000.0f);
    float alt2 = bme.readAltitude(101325.0f);
    assert(alt2 > 900.0f && alt2 < 1100.0f); // ~988m

    // Sea level pressure from altitude
    float slp = bme.seaLevelForAltitude(500.0f, 95000.0f);
    assert(slp > 100000.0f);

    // Sampling config
    bme.setSampling(Adafruit_BME280::MODE_FORCED);
    assert(bme.takeForcedMeasurement());

    printf("test_bme280: all assertions passed\n");
    return 0;
}
