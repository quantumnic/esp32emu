#pragma once
// esp32emu — mDNS responder mock
// Supports hostname registration, service advertisement, service TXT records,
// and service query (discovery) with injectable test results.

#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include "esp32emu_string.h"

struct MDNSService {
    std::string service;
    std::string proto;
    uint16_t port;
    std::map<std::string, std::string> txt;
};

struct MDNSResult {
    std::string hostname;
    uint32_t ip;       // network byte order
    uint16_t port;
    std::map<std::string, std::string> txt;
};

class MDNSResponder {
public:
    bool begin(const char* hostname) {
        hostname_ = hostname;
        started_ = true;
        fprintf(stderr, "[esp32emu] mDNS: registered hostname '%s.local'\n", hostname);
        return true;
    }

    void end() {
        started_ = false;
        services_.clear();
        hostname_.clear();
    }

    void addService(const char* service, const char* proto, uint16_t port) {
        MDNSService s;
        s.service = service;
        s.proto = proto;
        s.port = port;
        services_.push_back(s);
        fprintf(stderr, "[esp32emu] mDNS: added service _%s._%s on port %d\n",
                service, proto, port);
    }

    void addServiceTxt(const char* service, const char* proto,
                       const char* key, const char* value) {
        for (auto& s : services_) {
            if (s.service == service && s.proto == proto) {
                s.txt[key] = value;
                return;
            }
        }
    }

    // Service query / discovery
    int queryService(const char* service, const char* proto) {
        lastQueryService_ = service;
        lastQueryProto_ = proto;
        fprintf(stderr, "[esp32emu] mDNS: queryService _%s._%s → %zu results\n",
                service, proto, queryResults_.size());
        return (int)queryResults_.size();
    }

    String hostname(int idx) const {
        if (idx < 0 || idx >= (int)queryResults_.size()) return String("");
        return String(queryResults_[idx].hostname.c_str());
    }

    uint32_t IP(int idx) const {
        if (idx < 0 || idx >= (int)queryResults_.size()) return 0;
        return queryResults_[idx].ip;
    }

    uint16_t port(int idx) const {
        if (idx < 0 || idx >= (int)queryResults_.size()) return 0;
        return queryResults_[idx].port;
    }

    String txt(int idx, const char* key) const {
        if (idx < 0 || idx >= (int)queryResults_.size()) return String("");
        auto it = queryResults_[idx].txt.find(key);
        if (it == queryResults_[idx].txt.end()) return String("");
        return String(it->second.c_str());
    }

    int txtCount(int idx) const {
        if (idx < 0 || idx >= (int)queryResults_.size()) return 0;
        return (int)queryResults_[idx].txt.size();
    }

    void update() {}

    // ── Test helpers ──
    void _addQueryResult(const MDNSResult& r) { queryResults_.push_back(r); }
    void _clearQueryResults() { queryResults_.clear(); }
    const std::vector<MDNSService>& _getServices() const { return services_; }
    const std::string& _getHostname() const { return hostname_; }
    bool _isStarted() const { return started_; }

    void _reset() {
        started_ = false;
        hostname_.clear();
        services_.clear();
        queryResults_.clear();
    }

private:
    std::string hostname_;
    bool started_ = false;
    std::vector<MDNSService> services_;
    std::vector<MDNSResult> queryResults_;
    std::string lastQueryService_, lastQueryProto_;
};

extern MDNSResponder MDNS;
