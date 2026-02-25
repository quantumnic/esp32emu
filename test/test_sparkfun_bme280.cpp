#include "Arduino.h"
#include "SparkFun_BME280.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    // Test 1: Init I2C
    {
        BME280 sensor;
        assert(sensor.beginI2C(0x77));
    }

    // Test 2: Init SPI
    {
        BME280 sensor;
        assert(sensor.beginSPI(10));
    }

    // Test 3: begin() returns chip ID
    {
        BME280 sensor;
        assert(sensor.begin() == 0x60);
    }

    // Test 4: Temperature
    {
        BME280 sensor;
        sensor.begin();
        sensor.esp32emu_set_temperature(25.3f);
        assert(fabs(sensor.readTempC() - 25.3f) < 0.01f);
        // Fahrenheit
        float expected_f = 25.3f * 9.0f / 5.0f + 32.0f;
        assert(fabs(sensor.readTempF() - expected_f) < 0.1f);
    }

    // Test 5: Pressure
    {
        BME280 sensor;
        sensor.begin();
        sensor.esp32emu_set_pressure(100000.0f);
        assert(fabs(sensor.readFloatPressure() - 100000.0f) < 1.0f);
    }

    // Test 6: Humidity
    {
        BME280 sensor;
        sensor.begin();
        sensor.esp32emu_set_humidity(55.5f);
        assert(fabs(sensor.readFloatHumidity() - 55.5f) < 0.01f);
    }

    // Test 7: Altitude
    {
        BME280 sensor;
        sensor.begin();
        sensor.esp32emu_set_pressure(101325.0f);
        float alt = sensor.readFloatAltitudeMeters();
        assert(fabs(alt) < 1.0f); // ~0m at standard pressure
    }

    // Test 8: Altitude feet
    {
        BME280 sensor;
        sensor.begin();
        sensor.esp32emu_set_pressure(101325.0f);
        float alt_ft = sensor.readFloatAltitudeFeet();
        assert(fabs(alt_ft) < 3.0f);
    }

    // Test 9: Settings
    {
        BME280 sensor;
        sensor.settings.I2CAddress = 0x76;
        sensor.begin();
        sensor.setMode(BME280_MODE_FORCED);
        assert(sensor.getMode() == BME280_MODE_FORCED);
        sensor.setFilter(BME280_FILTER_4);
        sensor.setStandbyTime(BME280_STANDBY_500_MS);
        sensor.setTempOverSample(BME280_OVERSAMP_16X);
        sensor.setPressureOverSample(BME280_OVERSAMP_4X);
        sensor.setHumidityOverSample(BME280_OVERSAMP_2X);
    }

    // Test 10: Reference pressure
    {
        BME280 sensor;
        sensor.begin();
        sensor.setReferencePressure(100800.0f);
        assert(fabs(sensor.getReferencePressure() - 100800.0f) < 1.0f);
    }

    // Test 11: isMeasuring
    {
        BME280 sensor;
        sensor.begin();
        assert(!sensor.isMeasuring());
    }

    // Test 12: Reset
    {
        BME280 sensor;
        sensor.begin();
        sensor.reset(); // should not crash
    }

    printf("test_sparkfun_bme280: all assertions passed\n");
    return 0;
}
