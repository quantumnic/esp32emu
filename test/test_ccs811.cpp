// esp32emu test — CCS811 Air Quality Sensor
#include <cassert>
#include <cstdio>
#define ESP32EMU_MOCK
#include "Arduino.h"
#include "CCS811.h"

int main() {
    // Test init
    {
        CCS811 sensor;
        assert(!sensor.test_isInitialized());
        assert(sensor.begin());
        assert(sensor.test_isInitialized());
    }

    // Test beginWithStatus
    {
        CCS811 sensor;
        assert(sensor.beginWithStatus() == CCS811::CCS811_Stat_SUCCESS);
    }

    // Test data available and readings
    {
        CCS811 sensor;
        sensor.begin();
        assert(sensor.dataAvailable());
        sensor.readAlgorithmResults();
        assert(sensor.getCO2() == 400);
        assert(sensor.getTVOC() == 0);
    }

    // Test custom values
    {
        CCS811 sensor;
        sensor.begin();
        sensor.test_setCO2(1200);
        sensor.test_setTVOC(85);
        assert(sensor.getCO2() == 1200);
        assert(sensor.getTVOC() == 85);
    }

    // Test data not available
    {
        CCS811 sensor;
        sensor.begin();
        sensor.test_setDataReady(false);
        assert(!sensor.dataAvailable());
    }

    // Test drive mode
    {
        CCS811 sensor;
        sensor.begin();
        assert(sensor.setDriveMode(2));
        assert(sensor.test_getDriveMode() == 2);
        assert(!sensor.setDriveMode(5)); // invalid
    }

    // Test baseline
    {
        CCS811 sensor;
        sensor.begin();
        assert(sensor.getBaseline() == 0xA1B2);
        sensor.setBaseline(0x1234);
        assert(sensor.getBaseline() == 0x1234);
    }

    // Test environmental data
    {
        CCS811 sensor;
        sensor.begin();
        sensor.setEnvironmentalData(65.0f, 28.0f);
        assert(sensor.dataAvailable()); // still works
    }

    // Test error status
    {
        CCS811 sensor;
        sensor.begin();
        assert(!sensor.checkForStatusError());
        sensor.test_setError(true);
        assert(sensor.checkForStatusError());
    }

    // Test not initialized
    {
        CCS811 sensor;
        assert(!sensor.dataAvailable());
    }

    printf("test_ccs811: all assertions passed\n");
    return 0;
}
