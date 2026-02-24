#include "Adafruit_DRV2605.h"
#include <cassert>
#include <cstdio>

int main() {
    // Test 1: Init
    {
        Adafruit_DRV2605 drv;
        assert(drv.begin());
    }

    // Test 2: Waveform slots
    {
        Adafruit_DRV2605 drv;
        drv.begin();
        drv.setWaveform(0, 47);
        drv.setWaveform(1, 0);
        assert(drv._test_get_waveform(0) == 47);
        assert(drv._test_get_waveform(1) == 0);
    }

    // Test 3: Play/stop
    {
        Adafruit_DRV2605 drv;
        drv.begin();
        drv.setWaveform(0, 14);
        drv.setWaveform(1, 0);
        assert(!drv._test_is_playing());
        drv.go();
        assert(drv._test_is_playing());
        drv.stop();
        assert(!drv._test_is_playing());
    }

    // Test 4: Mode
    {
        Adafruit_DRV2605 drv;
        drv.begin();
        drv.setMode(Adafruit_DRV2605::DRV2605_MODE_REALTIME);
        assert(drv.getMode() == 0x05);
    }

    // Test 5: ERM/LRA
    {
        Adafruit_DRV2605 drv;
        drv.begin();
        assert(drv.isERM());
        drv.useLRA();
        assert(!drv.isERM());
        drv.useERM();
        assert(drv.isERM());
    }

    // Test 6: Library selection
    {
        Adafruit_DRV2605 drv;
        drv.begin();
        drv.selectLibrary(6);
        assert(drv.readLibrary() == 6);
    }

    // Test 7: Realtime value
    {
        Adafruit_DRV2605 drv;
        drv.begin();
        drv.setMode(Adafruit_DRV2605::DRV2605_MODE_REALTIME);
        drv.setRealtimeValue(200);
        assert(drv._test_get_rtp_value() == 200);
    }

    printf("test_drv2605: all assertions passed\n");
    return 0;
}
