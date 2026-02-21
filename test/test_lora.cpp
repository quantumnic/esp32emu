#include "Arduino.h"
#define ESP32EMU_LORA_NO_GLOBAL
#include "LoRa.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Test 1: Init
    {
        LoRaClass lora;
        assert(!lora.isInitialized());
        assert(lora.begin(915000000) == 1);
        assert(lora.isInitialized());
        assert(lora.getFrequency() == 915000000);
        lora.end();
        assert(!lora.isInitialized());
    }

    // Test 2: TX
    {
        LoRaClass lora;
        lora.begin(868000000);
        lora.beginPacket();
        lora.print("Hello LoRa");
        lora.endPacket();
        assert(lora.getTxCount() == 1);
        auto& pkt = lora.getLastTxPacket();
        assert(pkt.size() == 10);
        assert(memcmp(pkt.data(), "Hello LoRa", 10) == 0);
    }

    // Test 3: RX via inject
    {
        LoRaClass lora;
        lora.begin(433000000);
        lora.injectPacket("Test123", -70, 7.5f);

        int len = lora.parsePacket();
        assert(len == 7);
        assert(lora.available() == 7);
        assert(lora.packetRssi() == -70);
        assert(lora.packetSnr() > 7.0f && lora.packetSnr() < 8.0f);

        char buf[8] = {};
        for (int i = 0; i < 7; i++) buf[i] = (char)lora.read();
        assert(strcmp(buf, "Test123") == 0);
        assert(lora.available() == 0);
        assert(lora.read() == -1);
    }

    // Test 4: Configuration
    {
        LoRaClass lora;
        lora.begin(915000000);
        lora.setSpreadingFactor(12);
        assert(lora.getSpreadingFactor() == 12);
        lora.setSignalBandwidth(250000);
        assert(lora.getBandwidth() == 250000);
        lora.setTxPower(20);
        assert(lora.getTxPower() == 20);
        lora.enableCrc();
        assert(lora.getCrc());
        lora.disableCrc();
        assert(!lora.getCrc());
    }

    // Test 5: Multiple RX packets queued
    {
        LoRaClass lora;
        lora.begin(868000000);
        lora.injectPacket("PKT1");
        lora.injectPacket("PKT2");

        assert(lora.parsePacket() == 4);
        char b1[5] = {};
        for (int i = 0; i < 4; i++) b1[i] = (char)lora.read();
        assert(strcmp(b1, "PKT1") == 0);

        assert(lora.parsePacket() == 4);
        char b2[5] = {};
        for (int i = 0; i < 4; i++) b2[i] = (char)lora.read();
        assert(strcmp(b2, "PKT2") == 0);

        assert(lora.parsePacket() == 0); // no more
    }

    // Test 6: Callback
    {
        LoRaClass lora;
        lora.begin(868000000);
        int callbackLen = 0;
        lora.onReceive([&](int len) { callbackLen = len; });
        lora.injectPacket("CB", -80, 5.0f);
        assert(callbackLen == 2);
    }

    // Test 7: println
    {
        LoRaClass lora;
        lora.begin(868000000);
        lora.beginPacket();
        lora.println("Hi");
        lora.endPacket();
        auto& pkt = lora.getLastTxPacket();
        assert(pkt.size() == 4); // "Hi\r\n"
        assert(pkt[2] == '\r' && pkt[3] == '\n');
    }

    // Test 8: Peek
    {
        LoRaClass lora;
        lora.begin(868000000);
        lora.injectPacket("AB");
        lora.parsePacket();
        assert(lora.peek() == 'A');
        assert(lora.peek() == 'A'); // doesn't advance
        assert(lora.read() == 'A');
        assert(lora.peek() == 'B');
    }

    printf("test_lora: all assertions passed\n");
    return 0;
}
