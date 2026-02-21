#include "Adafruit_LIS3DH.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.5f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Init
    {
        Adafruit_LIS3DH lis;
        assert(!lis.isInitialized());
        assert(lis.begin(0x18));
        assert(lis.isInitialized());
    }

    // Test 2: Range
    {
        Adafruit_LIS3DH lis;
        lis.begin();
        lis.setRange(LIS3DH_RANGE_4_G);
        assert(lis.getRange() == LIS3DH_RANGE_4_G);
    }

    // Test 3: Data rate
    {
        Adafruit_LIS3DH lis;
        lis.begin();
        lis.setDataRate(LIS3DH_DATARATE_50_HZ);
        assert(lis.getDataRate() == LIS3DH_DATARATE_50_HZ);
    }

    // Test 4: Raw read
    {
        Adafruit_LIS3DH lis;
        lis.begin();
        lis.setRawData(1000, -2000, 16384);
        assert(lis.read());
        assert(lis.x == 1000);
        assert(lis.y == -2000);
        assert(lis.z == 16384);
    }

    // Test 5: Event (acceleration in m/s²)
    {
        Adafruit_LIS3DH lis;
        lis.begin();
        lis.setRange(LIS3DH_RANGE_2_G);
        lis.setRawData(0, 0, 16384); // 1G on Z
        sensors_event_t event;
        assert(lis.getEvent(&event));
        assert(near(event.acceleration.z, 9.806f, 1.0f));
        assert(near(event.acceleration.x, 0.0f, 0.1f));
    }

    // Test 6: Click detection
    {
        Adafruit_LIS3DH lis;
        lis.begin();
        lis.setClick(0x15, 80);
        assert(lis.getClick() == 0); // Not detected
        lis.setClickDetected(true);
        assert(lis.getClick() == 0x15);
    }

    // Test 7: Custom address
    {
        Adafruit_LIS3DH lis;
        lis.begin(0x19);
        assert(lis.getAddress() == 0x19);
    }

    printf("test_lis3dh: all assertions passed\n");
    return 0;
}
