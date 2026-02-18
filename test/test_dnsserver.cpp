#include "../include/DNSServer.h"
#include <cassert>
#include <cstdio>

int main() {
    // Test basic start/stop
    DNSServer dns;
    assert(!dns.isRunning());

    uint32_t ip = (192) | (168 << 8) | (4 << 16) | (1 << 24); // 192.168.4.1 in ESP format
    dns.start(53, "*", ip);
    assert(dns.isRunning());
    assert(dns.getPort() == 53);
    assert(dns.getDomain() == "*");
    assert(dns.getResolvedIP() == ip);

    // Test wildcard resolution
    assert(dns.resolve("google.com") == ip);
    assert(dns.resolve("anything.local") == ip);

    // Test specific domain
    DNSServer dns2;
    dns2.start(53, "mydevice.local", ip);
    assert(dns2.resolve("mydevice.local") == ip);
    assert(dns2.resolve("other.com") == 0);

    // Test processNextRequest counting
    assert(dns.getProcessCount() == 0);
    dns.processNextRequest();
    dns.processNextRequest();
    assert(dns.getProcessCount() == 2);

    // Test TTL and error code setters
    dns.setTTL(300);
    dns.setErrorReplyCode(2);

    // Test stop
    dns.stop();
    assert(!dns.isRunning());

    printf("test_dnsserver: all assertions passed\n");
    return 0;
}
