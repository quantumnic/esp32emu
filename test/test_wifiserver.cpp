#include "../include/WiFiServer.h"
#include "../include/WiFiClient.h"
#include <cassert>
#include <cstdio>
#include <thread>
#include <chrono>

int main() {
    // Test server creation and listening
    WiFiServer server(19900);
    server.begin();
    assert(server);

    // Test that available() returns empty client when no connections
    WiFiClient empty = server.available();
    assert(!empty);

    // Test actual TCP connection
    std::thread client_thread([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        WiFiClient c;
        int ok = c.connect("127.0.0.1", 19900);
        assert(ok);
        c.print("hello from client");
        // Keep connection alive until server reads
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        c.stop();
    });

    // Wait for client to connect
    WiFiClient incoming;
    for (int i = 0; i < 20; i++) {
        incoming = server.available();
        if (incoming) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    assert(incoming);

    // Read data from client — wait for data to arrive
    for (int i = 0; i < 50; i++) {
        if (incoming.available() > 0) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    assert(incoming.available() > 0);
    String data = incoming.readString();
    assert(data.indexOf("hello from client") >= 0);

    incoming.stop();
    client_thread.join();

    // Test stop
    server.stop();
    assert(!server);

    printf("test_wifiserver: all assertions passed\n");
    return 0;
}
