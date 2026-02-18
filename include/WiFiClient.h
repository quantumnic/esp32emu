#pragma once
#include <string>
#include <cstdint>
#include <cstring>
#include <vector>
#include "esp32emu_string.h"

// Forward — implemented in src/wificlient.cpp
class WiFiClient {
public:
    WiFiClient();
    WiFiClient(const char* host, uint16_t port, int fd);
    ~WiFiClient();

    // Move semantics (prevent double-close)
    WiFiClient(WiFiClient&& other) noexcept : fd_(other.fd_), timeout_ms_(other.timeout_ms_) {
        other.fd_ = -1;
    }
    WiFiClient& operator=(WiFiClient&& other) noexcept {
        if (this != &other) {
            stop();
            fd_ = other.fd_;
            timeout_ms_ = other.timeout_ms_;
            other.fd_ = -1;
        }
        return *this;
    }
    WiFiClient(const WiFiClient&) = delete;
    WiFiClient& operator=(const WiFiClient&) = delete;

    int connect(const char* host, uint16_t port);
    int connect(const char* host, uint16_t port, int timeout_ms);
    size_t write(const uint8_t* buf, size_t size);
    size_t write(uint8_t b) { return write(&b, 1); }
    size_t print(const char* s);
    size_t println(const char* s = "");
    int available();
    int read();
    int read(uint8_t* buf, size_t size);
    String readString();
    String readStringUntil(char terminator);
    bool connected();
    void stop();
    void setTimeout(int timeout_ms) { timeout_ms_ = timeout_ms; }
    operator bool() { return fd_ >= 0; }

    int fd() const { return fd_; }

private:
    int fd_ = -1;
    int timeout_ms_ = 5000;
};
