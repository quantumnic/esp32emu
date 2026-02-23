#include "MS5611.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 0.5f) { return std::fabs(a - b) < eps; }

int main() {
    // Init
    {
        MS5611 ms(0x77);
        assert(ms.begin());
        assert(ms.isConnected());
    }

    // Default readings
    {
        MS5611 ms;
        ms.begin();
        assert(ms.read() == 0);
        assert(near(ms.getTemperature(), 20.0f));
        assert(near(ms.getPressure(), 1013.25f));
    }

    // Custom readings
    {
        MS5611 ms;
        ms.begin();
        ms.setReadings(25.0f, 1000.0f);
        ms.read();
        assert(near(ms.getTemperature(), 25.0f));
        assert(near(ms.getPressure(), 1000.0f));
    }

    // Altitude calculation
    {
        MS5611 ms;
        ms.begin();
        ms.setPressure(1013.25f);
        float alt = ms.getAltitude();
        assert(near(alt, 0.0f, 1.0f)); // sea level
    }

    // High altitude
    {
        MS5611 ms;
        ms.begin();
        ms.setPressure(900.0f);
        float alt = ms.getAltitude();
        assert(alt > 900.0f && alt < 1100.0f); // ~988m
    }

    // Oversampling
    {
        MS5611 ms;
        ms.begin();
        ms.setOversampling(8);
        assert(ms.getOversampling() == 8);
    }

    // Address
    {
        MS5611 ms(0x76);
        ms.begin();
        assert(ms.getAddress() == 0x76);
    }

    printf("test_ms5611: all assertions passed\n");
    return 0;
}
