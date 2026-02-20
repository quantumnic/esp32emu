// esp32emu test — Adafruit SHT31 temperature/humidity sensor
#include "Adafruit_SHT31.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_SHT31 sht;

    // Not initialized
    assert(!sht._isInitialized());
    assert(std::isnan(sht.readTemperature()));
    assert(std::isnan(sht.readHumidity()));

    // Init
    assert(sht.begin(0x44));
    assert(sht._isInitialized());
    assert(sht._getAddr() == 0x44);

    // Default readings
    assert(fabs(sht.readTemperature() - 23.0f) < 0.01f);
    assert(fabs(sht.readHumidity() - 50.0f) < 0.01f);

    // readBoth
    float t, h;
    assert(sht.readBoth(&t, &h));
    assert(fabs(t - 23.0f) < 0.01f);
    assert(fabs(h - 50.0f) < 0.01f);

    // Inject values
    sht._setTemperature(35.5f);
    sht._setHumidity(75.0f);
    assert(fabs(sht.readTemperature() - 35.5f) < 0.01f);
    assert(fabs(sht.readHumidity() - 75.0f) < 0.01f);

    // Heater
    assert(!sht.isHeaterEnabled());
    sht.heater(true);
    assert(sht.isHeaterEnabled());
    sht.heater(false);
    assert(!sht.isHeaterEnabled());

    // Status and reset
    sht._setStatus(0x1234);
    assert(sht.readStatus() == 0x1234);
    sht.reset();

    printf("test_sht31: all assertions passed\n");
    return 0;
}
