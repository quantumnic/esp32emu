// esp32emu test — AS5600 Magnetic Rotary Encoder
#include <cassert>
#include <cstdio>
#include <cmath>
#define ESP32EMU_MOCK
#include "Arduino.h"
#include "AS5600.h"

int main() {
    // Test init
    {
        AS5600 enc;
        assert(!enc.test_isInitialized());
        assert(enc.begin());
        assert(enc.test_isInitialized());
        assert(enc.isConnected());
    }

    // Test angle reading
    {
        AS5600 enc;
        enc.begin();
        enc.test_setRawAngle(2048);
        assert(enc.rawAngle() == 2048);
        assert(enc.readAngle() == 2048);
        float deg = enc.readAngleDegrees();
        assert(fabs(deg - 180.0f) < 0.1f);
    }

    // Test full rotation
    {
        AS5600 enc;
        enc.begin();
        enc.test_setRawAngle(0);
        assert(fabs(enc.readAngleDegrees() - 0.0f) < 0.1f);
        enc.test_setRawAngle(4095);
        assert(fabs(enc.readAngleDegrees() - 359.9f) < 0.2f);
    }

    // Test magnet status
    {
        AS5600 enc;
        enc.begin();
        assert(enc.detectMagnet());
        assert(!enc.magnetTooStrong());
        assert(!enc.magnetTooWeak());

        enc.test_setMagnetTooStrong(true);
        assert(enc.magnetTooStrong());
        assert(enc.getMagnetStatus() & 0x08);

        enc.test_setMagnetTooStrong(false);
        enc.test_setMagnetTooWeak(true);
        assert(enc.magnetTooWeak());
        assert(enc.getMagnetStatus() & 0x10);
    }

    // Test AGC and magnitude
    {
        AS5600 enc;
        enc.begin();
        assert(enc.readAGC() == 128);
        enc.test_setAGC(200);
        assert(enc.readAGC() == 200);
        enc.test_setMagnitude(3000);
        assert(enc.readMagnitude() == 3000);
    }

    // Test position configuration
    {
        AS5600 enc;
        enc.begin();
        enc.setStartPosition(100);
        assert(enc.getStartPosition() == 100);
        enc.setStopPosition(3000);
        assert(enc.getStopPosition() == 3000);
        enc.setMaxAngle(2048);
        assert(enc.getMaxAngle() == 2048);
    }

    // Test direction
    {
        AS5600 enc;
        enc.begin();
        enc.setDirection(1);
        assert(enc.getDirection() == 1);
    }

    // Test cumulative position
    {
        AS5600 enc;
        enc.begin();
        enc.test_setCumulativePosition(12345);
        assert(enc.getCumulativePosition() == 12345);
        enc.resetCumulativePosition();
        assert(enc.getCumulativePosition() == 0);
        enc.resetCumulativePosition(-500);
        assert(enc.getCumulativePosition() == -500);
    }

    // Test 12-bit masking
    {
        AS5600 enc;
        enc.begin();
        enc.test_setRawAngle(0xFFFF);
        assert(enc.rawAngle() == 0x0FFF);
    }

    printf("test_as5600: all assertions passed\n");
    return 0;
}
