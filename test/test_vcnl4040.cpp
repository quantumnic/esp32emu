#include "Arduino.h"
#include "Adafruit_VCNL4040.h"
#include <cassert>
#include <cstdio>

int main() {
    Adafruit_VCNL4040 vcnl;
    assert(vcnl.begin());
    assert(vcnl.test_isInitialized());

    // Proximity
    vcnl.enableProximity(true);
    assert(vcnl.test_isProxEnabled());
    vcnl.test_setProximity(1234);
    assert(vcnl.getProximity() == 1234);

    // Proximity integration time
    vcnl.setProximityIntegrationTime(Adafruit_VCNL4040::VCNL4040_PS_IT_4T);
    assert(vcnl.getProximityIntegrationTime() == Adafruit_VCNL4040::VCNL4040_PS_IT_4T);

    // High resolution
    vcnl.setProximityHighResolution(true);
    assert(vcnl.getProximityHighResolution());

    // Ambient light
    vcnl.enableAmbientLight(true);
    assert(vcnl.test_isALSEnabled());
    vcnl.test_setLux(500);
    assert(vcnl.getLux() == 500);

    // ALS integration time
    vcnl.setAmbientIntegrationTime(Adafruit_VCNL4040::VCNL4040_ALS_IT_320MS);
    assert(vcnl.getAmbientIntegrationTime() == Adafruit_VCNL4040::VCNL4040_ALS_IT_320MS);

    // White light
    vcnl.test_setWhiteLight(300);
    assert(vcnl.getWhiteLight() == 300);

    // Proximity thresholds
    vcnl.setProximityLowThreshold(100);
    vcnl.setProximityHighThreshold(2000);
    assert(vcnl.getProximityLowThreshold() == 100);
    assert(vcnl.getProximityHighThreshold() == 2000);

    printf("test_vcnl4040: all assertions passed\n");
    return 0;
}
