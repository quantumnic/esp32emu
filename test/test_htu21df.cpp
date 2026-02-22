// test_htu21df: HTU21D-F humidity/temperature sensor mock tests
#include "Adafruit_HTU21DF.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    htu21df_test::reset();
    Adafruit_HTU21DF sensor;

    // Init
    assert(sensor.begin());
    assert(sensor.isConnected());

    // Default readings
    assert(fabs(sensor.readTemperature() - 22.5f) < 0.01f);
    assert(fabs(sensor.readHumidity() - 45.0f) < 0.01f);

    // Inject values
    htu21df_test::setTemperature(30.0f);
    htu21df_test::setHumidity(80.0f);
    assert(fabs(sensor.readTemperature() - 30.0f) < 0.01f);
    assert(fabs(sensor.readHumidity() - 80.0f) < 0.01f);

    // Unavailable sensor
    htu21df_test::reset();
    htu21df_test::setAvailable(false);
    Adafruit_HTU21DF sensor2;
    assert(!sensor2.begin());

    // Not begun - returns 0
    assert(sensor2.readTemperature() == 0.0f);
    assert(sensor2.readHumidity() == 0.0f);

    printf("test_htu21df: all assertions passed\n");
    return 0;
}
