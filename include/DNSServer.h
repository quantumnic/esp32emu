#pragma once
#include <cstdint>
#include <cstdio>
#include <string>
#include <functional>
#include <vector>

// DNS server mock for captive portal and local DNS resolution
class DNSServer {
public:
    void start(uint16_t port, const std::string& domain, uint32_t resolvedIP) {
        port_ = port;
        domain_ = domain;
        resolvedIP_ = resolvedIP;
        running_ = true;
        fprintf(stderr, "[esp32emu] DNSServer: started on port %d, redirecting '%s' -> %d.%d.%d.%d\n",
                port, domain.c_str(),
                (resolvedIP >> 0) & 0xFF, (resolvedIP >> 8) & 0xFF,
                (resolvedIP >> 16) & 0xFF, (resolvedIP >> 24) & 0xFF);
    }

    // Captive portal mode: resolve all domains to the given IP
    void start(uint16_t port, const char* domain, uint32_t resolvedIP) {
        start(port, std::string(domain), resolvedIP);
    }

    void stop() {
        running_ = false;
        fprintf(stderr, "[esp32emu] DNSServer: stopped\n");
    }

    void processNextRequest() {
        // In emulation, this is a no-op (no real UDP DNS traffic)
        // But we track that it was called for testing
        processCount_++;
    }

    void setErrorReplyCode(int code) { errorReplyCode_ = code; }
    void setTTL(uint32_t ttl) { ttl_ = ttl; }

    // Test helpers
    bool isRunning() const { return running_; }
    uint16_t getPort() const { return port_; }
    const std::string& getDomain() const { return domain_; }
    uint32_t getResolvedIP() const { return resolvedIP_; }
    int getProcessCount() const { return processCount_; }

    // Simulate a DNS query for testing
    uint32_t resolve(const std::string& hostname) const {
        if (domain_ == "*" || hostname == domain_) {
            return resolvedIP_;
        }
        return 0; // NXDOMAIN
    }

private:
    uint16_t port_ = 53;
    std::string domain_ = "*";
    uint32_t resolvedIP_ = 0;
    bool running_ = false;
    int errorReplyCode_ = 0;
    uint32_t ttl_ = 60;
    int processCount_ = 0;
};
