#include "RadioLib.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // SX1276 basic init and TX
    Module mod(5, 2, 14);
    SX1276 radio(&mod);
    assert(radio.begin(868.0, 125.0, 12, 5, 0x12, 14) == RADIOLIB_ERR_NONE);

    // Transmit
    assert(radio.transmit("Hello LoRa") == RADIOLIB_ERR_NONE);
    auto tx = radio._test_getLastTx();
    assert(tx.size() == 10);
    assert(memcmp(tx.data(), "Hello LoRa", 10) == 0);
    assert(radio._test_getTxCount() == 1);

    // Receive with injected data
    radio._test_injectRx("World");
    uint8_t buf[64];
    size_t len = sizeof(buf);
    assert(radio.receive(buf, len) == RADIOLIB_ERR_NONE);
    assert(len == 5);
    assert(memcmp(buf, "World", 5) == 0);

    // RX timeout when empty
    len = sizeof(buf);
    assert(radio.receive(buf, len) == RADIOLIB_ERR_RX_TIMEOUT);

    // Parameter setting
    assert(radio.setFrequency(915.0) == RADIOLIB_ERR_NONE);
    assert(radio._test_getFrequency() == 915.0f);
    assert(radio.setFrequency(50.0) == RADIOLIB_ERR_INVALID_FREQUENCY);

    assert(radio.setSpreadingFactor(7) == RADIOLIB_ERR_NONE);
    assert(radio._test_getSpreadingFactor() == 7);
    assert(radio.setSpreadingFactor(13) == RADIOLIB_ERR_INVALID_SPREADING_FACTOR);

    assert(radio.setOutputPower(10) == RADIOLIB_ERR_NONE);
    assert(radio._test_getPower() == 10);
    assert(radio.setOutputPower(20) == RADIOLIB_ERR_INVALID_OUTPUT_POWER);

    // RSSI/SNR
    radio._test_setRSSI(-120.0);
    radio._test_setSNR(5.0);
    assert(radio.getRSSI() == -120.0f);
    assert(radio.getSNR() == 5.0f);

    // Standby/sleep
    assert(radio.standby() == RADIOLIB_ERR_NONE);
    assert(radio.sleep() == RADIOLIB_ERR_NONE);

    // SX1262
    Module mod2(10, 3, 8);
    SX1262 radio2(&mod2);
    assert(radio2.begin(915.0) == RADIOLIB_ERR_NONE);
    assert(radio2.transmit("SX1262 test") == RADIOLIB_ERR_NONE);
    assert(radio2._test_getTxCount() == 1);

    radio2._test_injectRx("reply");
    uint8_t buf2[64];
    size_t len2 = sizeof(buf2);
    assert(radio2.receive(buf2, len2) == RADIOLIB_ERR_NONE);
    assert(len2 == 5);

    printf("test_radiolib: all assertions passed\n");
    return 0;
}
