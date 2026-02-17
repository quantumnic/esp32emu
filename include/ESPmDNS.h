#pragma once
#include <cstdio>
#include <cstdint>
#include "esp32emu_string.h"

class MDNSResponder {
public:
    bool begin(const char* hostname) {
        hostname_ = hostname;
        fprintf(stderr, "[esp32emu] mDNS: registered hostname '%s.local'\n", hostname);
        return true;
    }

    void addService(const char* service, const char* proto, uint16_t port) {
        fprintf(stderr, "[esp32emu] mDNS: added service _%s._%s on port %d\n",
                service, proto, port);
    }

    void addServiceTxt(const char* service, const char* proto,
                       const char* key, const char* value) {
        (void)service; (void)proto; (void)key; (void)value;
    }

    void update() {}
    void end() {}

private:
    std::string hostname_;
};

extern MDNSResponder MDNS;
