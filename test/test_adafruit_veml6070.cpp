// esp32emu test — Adafruit VEML6070 UV Light Sensor
#include <cassert>
#include <cstdio>
#define ESP32EMU_MOCK
#include "Arduino.h"
#include "Adafruit_VEML6070.h"

int main() {
    // Test init
    {
        Adafruit_VEML6070 uv;
        uv.begin(VEML6070_1_T);
        assert(uv.readUV() == 0); // default
    }

    // Test UV reading
    {
        Adafruit_VEML6070 uv;
        uv.begin();
        uv.test_setUV(1234);
        assert(uv.readUV() == 1234);
        uv.test_setUV(0);
        assert(uv.readUV() == 0);
        uv.test_setUV(65535);
        assert(uv.readUV() == 65535);
    }

    // Test integration time
    {
        Adafruit_VEML6070 uv;
        uv.begin(VEML6070_HALF_T);
        uv.setIntegrationTime(VEML6070_4_T);
        // no getter, just no crash
    }

    // Test sleep/wake
    {
        Adafruit_VEML6070 uv;
        uv.begin();
        assert(!uv.isSleeping());
        uv.sleep(true);
        assert(uv.isSleeping());
        uv.sleep(false);
        assert(!uv.isSleeping());
    }

    printf("test_adafruit_veml6070: all assertions passed\n");
    return 0;
}
