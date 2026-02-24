// test_bme68x.cpp — Adafruit BME68x sensor tests
#include "Adafruit_BME68x.h"
#include <cassert>
#include <cmath>
#include <cstdio>

int main() {
    // Test initialization
    {
        Adafruit_BME68x bme;
        assert(bme.begin());
    }

    // Test custom address
    {
        Adafruit_BME68x bme;
        assert(bme.begin(BME68X_SECONDARY_ADDRESS));
    }

    // Test reading defaults
    {
        Adafruit_BME68x bme;
        bme.begin();
        assert(bme.performReading());
        assert(fabs(bme.readTemperature() - 22.5f) < 0.01f);
        assert(fabs(bme.readPressure() - 101325.0f) < 1.0f);
        assert(fabs(bme.readHumidity() - 45.0f) < 0.01f);
        assert(fabs(bme.readGas() - 50000.0f) < 1.0f);
    }

    // Test custom values
    {
        Adafruit_BME68x bme;
        bme.begin();
        bme.setTestTemperature(30.0f);
        bme.setTestPressure(98000.0f);
        bme.setTestHumidity(80.0f);
        bme.setTestGasResistance(120000.0f);
        assert(bme.performReading());
        assert(fabs(bme.readTemperature() - 30.0f) < 0.01f);
        assert(fabs(bme.readPressure() - 98000.0f) < 1.0f);
        assert(fabs(bme.readHumidity() - 80.0f) < 0.01f);
        assert(fabs(bme.readGas() - 120000.0f) < 1.0f);
    }

    // Test oversampling config
    {
        Adafruit_BME68x bme;
        bme.begin();
        assert(bme.setTemperatureOversampling(BME68X_OS_8X));
        assert(bme.getTemperatureOversampling() == BME68X_OS_8X);
        assert(bme.setPressureOversampling(BME68X_OS_4X));
        assert(bme.getPressureOversampling() == BME68X_OS_4X);
        assert(bme.setHumidityOversampling(BME68X_OS_2X));
        assert(bme.getHumidityOversampling() == BME68X_OS_2X);
    }

    // Test IIR filter
    {
        Adafruit_BME68x bme;
        bme.begin();
        assert(bme.setIIRFilterSize(BME68X_FILTER_SIZE_3));
        assert(bme.getIIRFilterSize() == BME68X_FILTER_SIZE_3);
    }

    // Test gas heater
    {
        Adafruit_BME68x bme;
        bme.begin();
        assert(bme.setGasHeater(300, 100));
        assert(bme.getHeaterTemp() == 300);
        assert(bme.getHeaterTime() == 100);
        assert(bme.isGasEnabled());
    }

    // Test altitude calculation
    {
        Adafruit_BME68x bme;
        bme.begin();
        bme.setTestPressure(101325.0f); // sea level
        bme.performReading();
        float alt = bme.readAltitude(1013.25f);
        assert(fabs(alt) < 5.0f); // should be ~0 at sea level
    }

    // Test async reading flow
    {
        Adafruit_BME68x bme;
        bme.begin();
        uint32_t result = bme.beginReading();
        assert(result != 0);
        assert(bme.remainingReadingMillis() == 0);
        assert(bme.endReading() == 1);
        assert(fabs(bme.readTemperature() - 22.5f) < 0.01f);
    }

    // Test getData struct
    {
        Adafruit_BME68x bme;
        bme.begin();
        bme.setTestTemperature(25.0f);
        bme.performReading();
        bme68x_data d = bme.getData();
        assert(fabs(d.temperature - 25.0f) < 0.01f);
        assert(d.status == 0xB0);
    }

    // Test reading without init returns 0
    {
        Adafruit_BME68x bme;
        assert(!bme.performReading());
        assert(bme.readTemperature() == 0);
    }

    printf("test_bme68x: all assertions passed\n");
    return 0;
}
