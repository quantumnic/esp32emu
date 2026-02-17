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
    ~WiFiClient();

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
