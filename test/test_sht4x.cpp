#include "Adafruit_SHT4x.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_SHT4x sht;
    assert(sht.begin());

    // Default readings
    sensors_event_t humidity, temp;
    assert(sht.getEvent(&humidity, &temp));
    assert(std::fabs(temp.temperature - 22.5f) < 0.01f);
    assert(std::fabs(humidity.relative_humidity - 45.0f) < 0.01f);

    // Test helpers
    sht.esp32emu_set_temperature(35.2f);
    sht.esp32emu_set_humidity(78.5f);
    assert(sht.getEvent(&humidity, &temp));
    assert(std::fabs(temp.temperature - 35.2f) < 0.01f);
    assert(std::fabs(humidity.relative_humidity - 78.5f) < 0.01f);

    // Precision settings
    sht.setPrecision(SHT4X_MED_PRECISION);
    assert(sht.getPrecision() == SHT4X_MED_PRECISION);

    // Heater settings
    sht.setHeater(SHT4X_HIGH_HEATER_1S);
    assert(sht.getHeater() == SHT4X_HIGH_HEATER_1S);

    // Serial number
    sht.esp32emu_set_serial(0xAABBCCDD);
    assert(sht.readSerial() == 0xAABBCCDD);

    printf("test_sht4x: all assertions passed\n");
    return 0;
}
