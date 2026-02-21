#include "Adafruit_BMP3XX.h"
#include <cassert>
#include <cstdio>
#include <cmath>

static bool near(float a, float b, float eps = 1.0f) {
    return std::fabs(a - b) < eps;
}

int main() {
    // Test 1: Init
    {
        Adafruit_BMP3XX bmp;
        assert(!bmp.isInitialized());
        assert(bmp.begin_I2C());
        assert(bmp.isInitialized());
        assert(bmp.getAddress() == 0x77);
    }

    // Test 2: Default readings
    {
        Adafruit_BMP3XX bmp;
        bmp.begin_I2C();
        assert(bmp.performReading());
        assert(near(bmp.temperature, 22.0f));
        assert(near(bmp.pressure, 101325.0f, 10.0f));
    }

    // Test 3: Set readings
    {
        Adafruit_BMP3XX bmp;
        bmp.begin_I2C();
        bmp.setReadings(30.0f, 95000.0f);
        assert(near(bmp.temperature, 30.0f));
        assert(near(bmp.pressure, 95000.0f, 10.0f));
    }

    // Test 4: Altitude calculation
    {
        Adafruit_BMP3XX bmp;
        bmp.begin_I2C();
        bmp.pressure = 101325.0f;
        float alt = bmp.readAltitude(101325.0f);
        assert(near(alt, 0.0f, 1.0f)); // Sea level

        bmp.pressure = 95000.0f;
        alt = bmp.readAltitude(101325.0f);
        assert(alt > 400.0f && alt < 700.0f); // ~540m
    }

    // Test 5: Oversampling settings
    {
        Adafruit_BMP3XX bmp;
        bmp.begin_I2C();
        assert(bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X));
        assert(bmp.setPressureOversampling(BMP3_OVERSAMPLING_16X));
        assert(bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3));
        assert(bmp.setOutputDataRate(BMP3_ODR_50_HZ));
    }

    // Test 6: SPI init
    {
        Adafruit_BMP3XX bmp;
        assert(bmp.begin_SPI(10));
        assert(bmp.isInitialized());
    }

    // Test 7: Custom address
    {
        Adafruit_BMP3XX bmp;
        bmp.begin_I2C(0x76);
        assert(bmp.getAddress() == 0x76);
    }

    printf("test_bmp3xx: all assertions passed\n");
    return 0;
}
