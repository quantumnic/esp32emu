#include "Adafruit_ADS1X15.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    // Test ADS1015 (12-bit)
    Adafruit_ADS1015 ads1015;
    assert(ads1015.begin(0x48));
    ads1015.setGain(GAIN_ONE);
    assert(ads1015.getGain() == GAIN_ONE);

    ads1015.test_setValue(0, 1000);
    ads1015.test_setValue(1, 500);
    assert(ads1015.readADC_SingleEnded(0) == 1000);
    assert(ads1015.readADC_SingleEnded(1) == 500);
    assert(ads1015.readADC_Differential_0_1() == 500);

    // Voltage at GAIN_ONE: 4.096V range, 12-bit (2048 max)
    float v = ads1015.computeVolts(1024);
    assert(fabs(v - 2.048f) < 0.01f);

    // Test ADS1115 (16-bit)
    Adafruit_ADS1115 ads1115;
    assert(ads1115.begin(0x49));
    ads1115.setGain(GAIN_TWO);

    ads1115.test_setValue(0, 16384);
    assert(ads1115.readADC_SingleEnded(0) == 16384);

    // Voltage at GAIN_TWO: 2.048V range, 16-bit (32768 max)
    float v2 = ads1115.computeVolts(16384);
    assert(fabs(v2 - 1.024f) < 0.01f);

    // Comparator
    ads1115.test_setValue(2, 2000);
    ads1115.startComparator_SingleEnded(2, 1500);
    assert(ads1115.getLastConversionResults() == 2000);

    // Continuous mode
    ads1115.startADCReading(0, true);
    assert(ads1115.conversionComplete());

    printf("test_ads1x15: all assertions passed\n");
    return 0;
}
