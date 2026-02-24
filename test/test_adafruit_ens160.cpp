// esp32emu test — Adafruit ENS160 Air Quality Sensor
#include <cassert>
#include <cstdio>
#include <cmath>
#define ESP32EMU_MOCK
#include "Arduino.h"
#include "Adafruit_ENS160.h"

int main() {
    // Test init
    {
        Adafruit_ENS160 ens;
        assert(ens.begin());
        assert(ens.getFirmwareVersion() == 0x0207);
    }

    // Test mode setting
    {
        Adafruit_ENS160 ens;
        ens.begin();
        assert(ens.setMode(2)); // standard mode
        assert(ens.getMode() == 2);
        assert(ens.setMode(0)); // deep sleep
        assert(ens.getMode() == 0);
    }

    // Test AQI reading
    {
        Adafruit_ENS160 ens;
        ens.begin();
        assert(ens.getAQI() == 1); // default good
        ens.test_setAQI(4);
        assert(ens.getAQI() == 4);
    }

    // Test TVOC reading
    {
        Adafruit_ENS160 ens;
        ens.begin();
        assert(ens.getTVOC() == 0); // default
        ens.test_setTVOC(250);
        assert(ens.getTVOC() == 250);
    }

    // Test eCO2 reading
    {
        Adafruit_ENS160 ens;
        ens.begin();
        assert(ens.getECO2() == 400); // default baseline
        ens.test_setECO2(1200);
        assert(ens.getECO2() == 1200);
    }

    // Test data ready
    {
        Adafruit_ENS160 ens;
        ens.begin();
        assert(ens.dataReady());
        ens.test_setDataReady(false);
        assert(!ens.dataReady());
    }

    // Test compensation
    {
        Adafruit_ENS160 ens;
        ens.begin();
        ens.setTempCompensation(22.5f);
        ens.setRHCompensation(65.0f);
        // No getter, just ensure no crash
    }

    // Test status
    {
        Adafruit_ENS160 ens;
        assert(ens.getStatus() == 0x00); // not initialized
        ens.begin();
        assert(ens.getStatus() == 0x80); // initialized
    }

    printf("test_adafruit_ens160: all assertions passed\n");
    return 0;
}
