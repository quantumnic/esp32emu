#include "ESPmDNS.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Test begin/end
    {
        MDNS._reset();
        assert(!MDNS._isStarted());
        assert(MDNS.begin("my-esp32"));
        assert(MDNS._isStarted());
        assert(MDNS._getHostname() == "my-esp32");
        MDNS.end();
        assert(!MDNS._isStarted());
    }

    // Test service registration
    {
        MDNS._reset();
        MDNS.begin("sensor-node");
        MDNS.addService("http", "tcp", 80);
        MDNS.addService("mqtt", "tcp", 1883);
        assert(MDNS._getServices().size() == 2);
        assert(MDNS._getServices()[0].service == "http");
        assert(MDNS._getServices()[0].port == 80);
        assert(MDNS._getServices()[1].service == "mqtt");
        assert(MDNS._getServices()[1].port == 1883);
    }

    // Test TXT records
    {
        MDNS._reset();
        MDNS.begin("sensor");
        MDNS.addService("http", "tcp", 80);
        MDNS.addServiceTxt("http", "tcp", "path", "/api");
        MDNS.addServiceTxt("http", "tcp", "version", "1.0");
        assert(MDNS._getServices()[0].txt.size() == 2);
        assert(MDNS._getServices()[0].txt.at("path") == "/api");
        assert(MDNS._getServices()[0].txt.at("version") == "1.0");
    }

    // Test service query with injected results
    {
        MDNS._reset();
        MDNS.begin("client");

        MDNSResult r1;
        r1.hostname = "sensor-1";
        r1.ip = 0x0100A8C0; // 192.168.0.1
        r1.port = 80;
        r1.txt["type"] = "temperature";
        MDNS._addQueryResult(r1);

        MDNSResult r2;
        r2.hostname = "sensor-2";
        r2.ip = 0x0200A8C0;
        r2.port = 8080;
        MDNS._addQueryResult(r2);

        int n = MDNS.queryService("http", "tcp");
        assert(n == 2);

        String h = MDNS.hostname(0);
        assert(strcmp(h.c_str(), "sensor-1") == 0);
        assert(MDNS.port(0) == 80);
        assert(MDNS.IP(0) == 0x0100A8C0);
        assert(strcmp(MDNS.txt(0, "type").c_str(), "temperature") == 0);

        assert(strcmp(MDNS.hostname(1).c_str(), "sensor-2") == 0);
        assert(MDNS.port(1) == 8080);
    }

    // Test query with no results
    {
        MDNS._reset();
        MDNS.begin("node");
        int n = MDNS.queryService("mqtt", "tcp");
        assert(n == 0);
    }

    // Test out of bounds access
    {
        MDNS._reset();
        MDNS.begin("node");
        assert(MDNS.port(-1) == 0);
        assert(MDNS.port(99) == 0);
        assert(MDNS.IP(-1) == 0);
        assert(MDNS.txtCount(-1) == 0);
    }

    // Test clear query results
    {
        MDNS._reset();
        MDNS.begin("node");
        MDNSResult r;
        r.hostname = "test";
        r.ip = 0;
        r.port = 80;
        MDNS._addQueryResult(r);
        assert(MDNS.queryService("http", "tcp") == 1);
        MDNS._clearQueryResults();
        assert(MDNS.queryService("http", "tcp") == 0);
    }

    printf("test_espmdns: all assertions passed\n");
    return 0;
}
