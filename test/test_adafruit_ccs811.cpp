// esp32emu test — Adafruit_CCS811
#include <cassert>
#include <cstdio>
#include <cmath>
#define ESP32EMU_MOCK
#include "Arduino.h"
#include "Adafruit_CCS811.h"

int main() {
    // Test init
    {
        Adafruit_CCS811 sensor;
        assert(!sensor.isBegun());
        assert(sensor.begin());
        assert(sensor.isBegun());
    }

    // Test custom address
    {
        Adafruit_CCS811 sensor;
        assert(sensor.begin(0x5B));
        assert(sensor.isBegun());
    }

    // Test eCO2 + TVOC
    {
        Adafruit_CCS811 sensor;
        sensor.begin();
        assert(sensor.geteCO2() == 400); // default
        assert(sensor.getTVOC() == 0);   // default
        sensor.setECO2(1200);
        sensor.setTVOC(80);
        assert(sensor.geteCO2() == 1200);
        assert(sensor.getTVOC() == 80);
    }

    // Test data availability
    {
        Adafruit_CCS811 sensor;
        sensor.begin();
        assert(sensor.available());
        sensor.setDataReady(false);
        assert(!sensor.available());
    }

    // Test readData
    {
        Adafruit_CCS811 sensor;
        sensor.begin();
        assert(sensor.readData() == 0); // success
    }

    // Test environmental data
    {
        Adafruit_CCS811 sensor;
        sensor.begin();
        sensor.setEnvironmentalData(65.0f, 22.0f);
        assert(fabs(sensor.calculateTemperature() - 22.0f) < 0.1f);
    }

    // Test drive mode
    {
        Adafruit_CCS811 sensor;
        sensor.begin();
        sensor.setDriveMode(Adafruit_CCS811::CCS811_DRIVE_MODE_10SEC);
        assert(sensor.getDriveMode() == Adafruit_CCS811::CCS811_DRIVE_MODE_10SEC);
    }

    // Test baseline
    {
        Adafruit_CCS811 sensor;
        sensor.begin();
        sensor.setBaseline(0xABCD);
        assert(sensor.getBaseline() == 0xABCD);
    }

    // Test SW reset
    {
        Adafruit_CCS811 sensor;
        sensor.begin();
        sensor.setECO2(2000);
        sensor.setTVOC(500);
        sensor.SWReset();
        assert(sensor.geteCO2() == 400);
        assert(sensor.getTVOC() == 0);
    }

    // Test error
    {
        Adafruit_CCS811 sensor;
        sensor.begin();
        assert(!sensor.checkError());
        sensor.setError(true);
        assert(sensor.checkError());
    }

    printf("test_adafruit_ccs811: all assertions passed\n");
    return 0;
}
