#include "Adafruit_MPR121.h"
#include <cassert>
#include <cstdio>

int main() {
    // Test 1: Init
    {
        Adafruit_MPR121 cap;
        assert(!cap.isInitialized());
        assert(cap.begin(0x5A));
        assert(cap.isInitialized());
        assert(cap.getAddress() == 0x5A);
    }

    // Test 2: Touch state
    {
        Adafruit_MPR121 cap;
        cap.begin();
        assert(cap.touched() == 0);
        cap.setTouched(0x00F);
        assert(cap.touched() == 0x00F);
    }

    // Test 3: Individual channels
    {
        Adafruit_MPR121 cap;
        cap.begin();
        cap.setChannelTouched(0, true);
        cap.setChannelTouched(5, true);
        assert(cap.touched() == ((1 << 0) | (1 << 5)));
        cap.setChannelTouched(0, false);
        assert(cap.touched() == (1 << 5));
    }

    // Test 4: Filtered/baseline data
    {
        Adafruit_MPR121 cap;
        cap.begin();
        cap.setFilteredData(3, 150);
        cap.setBaselineData(3, 200);
        assert(cap.filteredData(3) == 150);
        assert(cap.baselineData(3) == 200);
        assert(cap.filteredData(12) == 0); // Out of range
    }

    // Test 5: Thresholds
    {
        Adafruit_MPR121 cap;
        cap.begin();
        assert(cap.getTouchThreshold() == 12);
        assert(cap.getReleaseThreshold() == 6);
        cap.setThresholds(20, 10);
        assert(cap.getTouchThreshold() == 20);
        assert(cap.getReleaseThreshold() == 10);
    }

    // Test 6: 12-bit mask limit
    {
        Adafruit_MPR121 cap;
        cap.begin();
        cap.setTouched(0xFFFF);
        assert(cap.touched() == 0x0FFF);
    }

    // Test 7: Custom address
    {
        Adafruit_MPR121 cap;
        cap.begin(0x5B);
        assert(cap.getAddress() == 0x5B);
    }

    printf("test_mpr121: all assertions passed\n");
    return 0;
}
