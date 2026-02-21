// esp32emu test — RCSwitch 433MHz RF
#include <cassert>
#include <cstdio>
#define ESP32EMU_MOCK
#include "Arduino.h"
#include "RCSwitch.h"

int main() {
    // Test enable/disable
    {
        RCSwitch rc;
        assert(!rc.test_isRxEnabled());
        assert(!rc.test_isTxEnabled());
        rc.enableReceive(2);
        rc.enableTransmit(4);
        assert(rc.test_isRxEnabled());
        assert(rc.test_isTxEnabled());
        rc.disableReceive();
        rc.disableTransmit();
        assert(!rc.test_isRxEnabled());
        assert(!rc.test_isTxEnabled());
    }

    // Test receive
    {
        RCSwitch rc;
        rc.enableReceive(2);
        assert(!rc.available());
        rc.test_injectReceived(0x123ABC, 24, 1, 350);
        assert(rc.available());
        assert(rc.getReceivedValue() == 0x123ABC);
        assert(rc.getReceivedBitlength() == 24);
        assert(rc.getReceivedProtocol() == 1);
        assert(rc.getReceivedDelay() == 350);
        rc.resetAvailable();
        assert(!rc.available());
    }

    // Test transmit
    {
        RCSwitch rc;
        rc.enableTransmit(4);
        rc.send(0xABCDEF, 24);
        assert(rc.test_getLastSentCode() == 0xABCDEF);
        assert(rc.test_getLastSentLength() == 24);
        assert(rc.test_getSendCount() == 1);
        rc.send(0x111, 12);
        assert(rc.test_getSendCount() == 2);
    }

    // Test protocol/timing settings
    {
        RCSwitch rc;
        rc.setProtocol(2);
        rc.setPulseLength(400);
        rc.setRepeatTransmit(5);
        // No crash = success for config
    }

    // Test switch helpers
    {
        RCSwitch rc;
        rc.enableTransmit(4);
        rc.switchOn("11001", "01000");
        rc.switchOff("11001", "01000");
        assert(rc.test_getSendCount() == 2);
    }

    printf("test_rcswitch: all assertions passed\n");
    return 0;
}
