#include "AsyncUDP.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Listen
    {
        AsyncUDP udp;
        assert(udp.listen(1234));
        assert(udp.isListening());
        assert(udp.getPort() == 1234);
    }

    // Close
    {
        AsyncUDP udp;
        udp.listen(1234);
        udp.close();
        assert(!udp.isListening());
    }

    // Send data
    {
        AsyncUDP udp;
        udp.write("hello");
        assert(udp.getTotalSent() == 5);
        auto& d = udp.getSentData();
        assert(d.size() == 5);
        assert(memcmp(d.data(), "hello", 5) == 0);
    }

    // Inject packet + handler
    {
        bool called = false;
        std::string received;

        AsyncUDP udp;
        udp.listen(5000);
        udp.onPacket([&](AsyncUDPPacket& pkt) {
            called = true;
            received.assign((char*)pkt.data(), pkt.length());
            assert(strcmp(pkt.remoteIP(), "10.0.0.1") == 0);
            assert(pkt.remotePort() == 9999);
            pkt.write("reply");
        });

        udp.injectPacket("test message", "10.0.0.1", 9999);
        assert(called);
        assert(received == "test message");
    }

    // Connect
    {
        AsyncUDP udp;
        IPAddress_t ip(192, 168, 1, 1);
        assert(udp.connect(ip, 8080));
        assert(udp.isConnected());
    }

    // Multicast
    {
        AsyncUDP udp;
        IPAddress_t mcast(239, 1, 2, 3);
        assert(udp.listenMulticast(mcast, 7777));
        assert(udp.isMulticast());
        assert(udp.isListening());
    }

    // Broadcast
    {
        AsyncUDP udp;
        udp.broadcast((const uint8_t*)"bc", 2);
        assert(udp.getTotalSent() == 2);
    }

    printf("test_asyncudp: all assertions passed\n");
    return 0;
}
