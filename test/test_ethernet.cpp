#include "Arduino.h"
#define ESP32EMU_ETHERNET_NO_GLOBAL
#include "Ethernet.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Test 1: Ethernet DHCP begin
    {
        EthernetClass eth;
        uint8_t mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
        assert(eth.begin(mac) == 1);
        assert(eth.linkStatus() == 1);
        assert(eth.hardwareStatus() == 1);
        assert(eth.isInitialized());
        auto ip = eth.localIP();
        assert(ip[0] == 192 && ip[1] == 168 && ip[2] == 1 && ip[3] == 100);
    }

    // Test 2: Ethernet static IP
    {
        EthernetClass eth;
        uint8_t mac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
        IPAddressEth ip(10, 0, 0, 50);
        eth.begin(mac, ip);
        assert(eth.linkStatus() == 1);
        auto lip = eth.localIP();
        assert(lip[0] == 10 && lip[3] == 50);
    }

    // Test 3: MAC readback
    {
        EthernetClass eth;
        uint8_t mac[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
        eth.begin(mac);
        uint8_t out[6];
        eth.MACAddress(out);
        assert(memcmp(mac, out, 6) == 0);
    }

    // Test 4: Link status control
    {
        EthernetClass eth;
        uint8_t mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
        eth.begin(mac);
        assert(eth.linkStatus() == 1);
        eth.setLinkUp(false);
        assert(eth.linkStatus() == 0);
    }

    // Test 5: EthernetClient connect/send/receive
    {
        EthernetClient client;
        assert(!client.connected());
        assert(client.connect("192.168.1.1", 80) == 1);
        assert(client.connected());
        client.print("GET / HTTP/1.1\r\n");
        assert(client.getTxString().find("GET /") != std::string::npos);

        client.injectRx("HTTP/1.1 200 OK\r\n");
        assert(client.available() == 17);
        char buf[5];
        for (int i = 0; i < 4; i++) buf[i] = (char)client.read();
        buf[4] = '\0';
        assert(strcmp(buf, "HTTP") == 0);

        client.stop();
        assert(!client.connected());
    }

    // Test 6: EthernetClient bulk read
    {
        EthernetClient client;
        client.connect("example.com", 443);
        client.injectRx("Hello World");
        uint8_t buf[32];
        int n = client.read(buf, 5);
        assert(n == 5);
        assert(memcmp(buf, "Hello", 5) == 0);
        assert(client.available() == 6);
    }

    // Test 7: EthernetClient peek
    {
        EthernetClient client;
        client.connect("test.local", 8080);
        client.injectRx("AB");
        assert(client.peek() == 'A');
        assert(client.read() == 'A');
        assert(client.peek() == 'B');
    }

    // Test 8: EthernetServer
    {
        EthernetServer server(80);
        assert(server.getPort() == 80);
        server.begin();
        assert(server.isRunning());

        // No pending clients
        auto c1 = server.available();
        assert(!c1.connected());

        // Inject a client
        EthernetClient injected;
        injected.connect("client.local", 12345);
        server.injectClient(injected);
        auto c2 = server.available();
        assert(c2.connected());
    }

    // Test 9: IPAddressEth toString
    {
        IPAddressEth ip(192, 168, 0, 1);
        assert(ip.toString() == "192.168.0.1");
    }

    // Test 10: EthernetClient println
    {
        EthernetClient client;
        client.connect("host", 80);
        client.println("Hello");
        auto tx = client.getTxString();
        assert(tx == "Hello\r\n");
        client.clearTx();
        assert(client.getTxString().empty());
    }

    printf("test_ethernet: all assertions passed\n");
    return 0;
}
