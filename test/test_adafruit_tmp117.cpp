// esp32emu — test_adafruit_tmp117: TMP117 high-accuracy temperature sensor mock test
#include "Adafruit_TMP117.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    Adafruit_TMP117 tmp;

    // Not initialized yet
    assert(!tmp.esp32emu_is_initialized());
    assert(tmp.begin(0x48));
    assert(tmp.esp32emu_is_initialized());

    // Default reading
    assert(tmp.dataReady());
    float t = tmp.readTempC();
    assert(fabs(t - 23.0f) < 0.01f);
    assert(!tmp.dataReady()); // consumed

    // Inject test value
    tmp.esp32emu_set_temperature(36.6f);
    assert(tmp.dataReady());
    assert(fabs(tmp.readTempC() - 36.6f) < 0.01f);

    // Device ID
    assert(tmp.getDeviceID() == 0x0117);

    // Rate / averaging
    tmp.setRate(TMP117_RATE_64_HZ);
    assert(tmp.getRate() == TMP117_RATE_64_HZ);
    tmp.setAveraging(TMP117_AVERAGE_64X);
    assert(tmp.getAveraging() == TMP117_AVERAGE_64X);

    // Soft reset restores defaults
    tmp.softReset();
    assert(tmp.getRate() == TMP117_RATE_4_HZ);
    assert(tmp.getAveraging() == TMP117_AVERAGE_8X);

    // Thresholds and alerts
    tmp.setHighThreshold(30.0f);
    tmp.setLowThreshold(10.0f);
    assert(fabs(tmp.getHighThreshold() - 30.0f) < 0.01f);
    assert(fabs(tmp.getLowThreshold() - 10.0f) < 0.01f);

    tmp.esp32emu_set_temperature(25.0f);
    assert(tmp.getAlert() == TMP117_ALERT_NONE);

    tmp.esp32emu_set_temperature(35.0f);
    assert(tmp.getAlert() == TMP117_ALERT_HIGH);

    tmp.esp32emu_set_temperature(5.0f);
    assert(tmp.getAlert() == TMP117_ALERT_LOW);

    // Offset
    tmp.setOffset(1.5f);
    assert(fabs(tmp.getOffset() - 1.5f) < 0.01f);

    // Alternate address
    Adafruit_TMP117 tmp2;
    assert(tmp2.begin(0x49));

    printf("test_adafruit_tmp117: all assertions passed\n");
    return 0;
}
