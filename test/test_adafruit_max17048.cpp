// esp32emu — test_adafruit_max17048: MAX17048 battery fuel gauge mock test
#include "Adafruit_MAX1704X.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_MAX17048 gauge;

    assert(!gauge.test_isBegun());
    assert(gauge.begin());
    assert(gauge.test_isBegun());

    // Default readings
    assert(fabs(gauge.cellVoltage() - 3.7f) < 0.01f);
    assert(fabs(gauge.cellPercent() - 75.0f) < 0.01f);
    assert(fabs(gauge.chargeRate() - 0.5f) < 0.01f);

    // Not in alert at 75%
    assert(!gauge.isActiveAlert());

    // Set low battery
    gauge.test_setPercent(5.0f);
    assert(fabs(gauge.cellPercent() - 5.0f) < 0.01f);
    assert(gauge.isActiveAlert()); // <10% triggers alert

    // Set custom voltage
    gauge.test_setVoltage(4.2f);
    assert(fabs(gauge.cellVoltage() - 4.2f) < 0.01f);

    // Set custom rate
    gauge.test_setRate(-1.5f);
    assert(fabs(gauge.chargeRate() - (-1.5f)) < 0.01f);

    // Quick start, sleep, wake
    gauge.quickStart();
    gauge.sleep();
    gauge.wake();

    // Alert voltages
    gauge.setAlertVoltages(3.0f, 4.3f);
    gauge.setActivityThreshold(0.1f);

    printf("test_adafruit_max17048: all assertions passed\n");
    return 0;
}
