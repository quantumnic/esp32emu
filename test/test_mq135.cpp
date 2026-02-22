// esp32emu — MQ135 gas sensor test
#include "Arduino.h"
#include "MQ135.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    MQ135 sensor(34);
    assert(sensor.getPin() == 34);

    // Inject a raw ADC value
    sensor._injectRaw(200);

    // Resistance calculation
    float r = sensor.getResistance();
    // (1023/200 - 1) * 10 = (5.115 - 1) * 10 = 41.15
    assert(fabs(r - 41.15f) < 0.1f);

    // PPM should be a positive finite value
    float ppm = sensor.getPPM();
    assert(ppm > 0 && std::isfinite(ppm));

    // Corrected PPM with temperature/humidity
    float cppm = sensor.getCorrectedPPM(25.0f, 50.0f);
    assert(cppm > 0 && std::isfinite(cppm));

    // RZero should be positive
    float rz = sensor.getRZero();
    assert(rz > 0 && std::isfinite(rz));

    float crz = sensor.getCorrectedRZero(22.0f, 45.0f);
    assert(crz > 0 && std::isfinite(crz));

    // Correction factor
    float cf1 = sensor.getCorrectionFactor(15.0f, 33.0f);
    assert(std::isfinite(cf1));
    float cf2 = sensor.getCorrectionFactor(25.0f, 60.0f);
    assert(std::isfinite(cf2));
    // Different conditions should give different factors
    assert(fabs(cf1 - cf2) > 0.001f);

    // Different raw value → different PPM
    sensor._injectRaw(500);
    float ppm2 = sensor.getPPM();
    assert(ppm2 != ppm);

    // Higher raw ADC → lower resistance → higher PPM
    sensor._injectRaw(800);
    float ppm3 = sensor.getPPM();
    assert(ppm3 > ppm2);

    printf("test_mq135: all assertions passed\n");
    return 0;
}
