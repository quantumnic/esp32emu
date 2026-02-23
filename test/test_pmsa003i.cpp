#include "Adafruit_PMSA003I.h"
#include <cassert>
#include <cstdio>

int main() {
    Adafruit_PM25AQI aqi;
    assert(aqi.begin_I2C());

    // Default readings
    PM25_AQI_Data data;
    assert(aqi.read(&data));
    assert(data.pm25_standard == 12);
    assert(data.pm100_env == 18);
    assert(data.particles_03um == 1200);

    // Set PM2.5
    aqi.esp32emu_set_pm25(35, 38);
    assert(aqi.read(&data));
    assert(data.pm25_standard == 35);
    assert(data.pm25_env == 38);

    // Set PM10 (coarse)
    aqi.esp32emu_set_pm100(55, 60);
    assert(aqi.read(&data));
    assert(data.pm100_standard == 55);
    assert(data.pm100_env == 60);

    // Set PM1.0
    aqi.esp32emu_set_pm10(8, 9);
    assert(aqi.read(&data));
    assert(data.pm10_standard == 8);
    assert(data.pm10_env == 9);

    // Set full data struct
    PM25_AQI_Data custom = {};
    custom.pm25_standard = 150;
    custom.pm25_env = 155;
    custom.particles_25um = 500;
    aqi.esp32emu_set_data(custom);
    assert(aqi.read(&data));
    assert(data.pm25_standard == 150);
    assert(data.particles_25um == 500);

    // UART init path
    Adafruit_PM25AQI aqi2;
    assert(aqi2.begin_UART(nullptr));
    assert(aqi2.read(&data));

    printf("test_pmsa003i: all assertions passed\n");
    return 0;
}
