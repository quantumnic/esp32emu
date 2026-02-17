#pragma once
#include <string>
#include <map>
#include <cstdint>
#include "esp32emu_string.h"
#include "WiFiClient.h"

class HTTPClient {
public:
    bool begin(const char* url);
    bool begin(const String& url) { return begin(url.c_str()); }
    void end();

    void addHeader(const char* name, const char* value);
    void addHeader(const String& name, const String& value) {
        addHeader(name.c_str(), value.c_str());
    }

    int GET();
    int POST(const char* payload);
    int POST(const String& payload) { return POST(payload.c_str()); }
    int PUT(const char* payload);
    int PUT(const String& payload) { return PUT(payload.c_str()); }

    int getSize() { return (int)response_body_.length(); }
    String getString() { return String(response_body_.c_str()); }
    String errorToString(int code);

    void setTimeout(uint16_t ms) { timeout_ms_ = ms; }

private:
    int sendRequest(const char* method, const char* payload = nullptr);

    std::string host_;
    uint16_t port_ = 80;
    std::string path_;
    std::map<std::string, std::string> headers_;
    std::string response_body_;
    uint16_t timeout_ms_ = 5000;
};
