#include "Adafruit_BMP280.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_BMP280 bmp;
    assert(!bmp.isInitialized());

    assert(bmp.begin(0x76));
    assert(bmp.isInitialized());

    // Default values
    assert(std::fabs(bmp.readTemperature() - 25.0f) < 0.01f);
    assert(std::fabs(bmp.readPressure() - 101325.0f) < 1.0f);

    // Altitude at sea level pressure should be ~0
    float alt = bmp.readAltitude(1013.25f);
    assert(std::fabs(alt) < 1.0f);

    // Set temperature
    bmp.setTemperature(30.5f);
    assert(std::fabs(bmp.readTemperature() - 30.5f) < 0.01f);

    // Set lower pressure → higher altitude
    bmp.setPressure(90000.0f); // ~1000m
    float highAlt = bmp.readAltitude(1013.25f);
    assert(highAlt > 500.0f && highAlt < 1500.0f);

    // Set higher pressure → below sea level
    bmp.setPressure(102000.0f);
    float lowAlt = bmp.readAltitude(1013.25f);
    assert(lowAlt < 100.0f);

    // setSampling should not crash
    bmp.setSampling();

    printf("test_bmp280: all assertions passed\n");
    return 0;
}
