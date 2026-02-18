// Test WiFiUDP mock — real loopback UDP sockets
#include "Arduino.h"
#include "WiFi.h"
#include "WiFiUDP.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <thread>
#include <chrono>

int main() {
    WiFi.begin("test", "test");

    // Create sender and receiver on different ports
    WiFiUDP receiver;
    WiFiUDP sender;

    assert(receiver.begin(19800));
    assert(sender.begin(19801));

    // Send a packet
    sender.beginPacket("127.0.0.1", 19800);
    const char* msg = "Hello ESP-NOW!";
    sender.write((const uint8_t*)msg, strlen(msg));
    assert(sender.endPacket() == 1);

    // Small delay for delivery
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // Receive
    int len = receiver.parsePacket();
    assert(len == (int)strlen(msg));
    assert(receiver.available() == len);

    char buf[64] = {};
    receiver.read(buf, sizeof(buf));
    assert(strcmp(buf, msg) == 0);

    // Remote info
    assert(receiver.remotePort() == 19801);

    // Test print helper
    sender.beginPacket("127.0.0.1", 19800);
    sender.print("ping");
    assert(sender.endPacket() == 1);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    assert(receiver.parsePacket() == 4);

    // Multiple reads
    assert(receiver.read() == 'p');
    assert(receiver.read() == 'i');
    assert(receiver.available() == 2);

    sender.stop();
    receiver.stop();

    printf("test_udp: all assertions passed\n");
    return 0;
}
