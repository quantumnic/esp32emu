#include "Adafruit_NAU7802.h"
#include <cassert>
#include <cstdio>

int main() {
    Adafruit_NAU7802 nau;

    assert(nau.begin());
    assert(nau._test_isInitialized());

    // Config
    assert(nau.setLDO(4));
    assert(nau.setGain(128));
    assert(nau.setRate(80));
    assert(nau._test_getGain() == 128);
    assert(nau._test_getRate() == 80);
    assert(nau._test_getLDO() == 4);

    // Calibration
    assert(nau.calibrate(0));
    assert(nau.waitForCalibration());
    assert(nau._test_isCalibrated());

    // Not available until data injected
    assert(!nau.available());

    // Inject value (simulating load cell reading)
    nau._test_setValue(123456);
    assert(nau.available());
    int32_t val = nau.read();
    assert(val == 123456);
    assert(!nau.available()); // consumed

    // Multiple readings
    nau._test_setValue(-50000);
    assert(nau.available());
    assert(nau.read() == -50000);

    // Reset
    assert(nau.reset());
    assert(!nau._test_isCalibrated());

    printf("test_nau7802: all assertions passed\n");
    return 0;
}
