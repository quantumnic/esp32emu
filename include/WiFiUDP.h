#pragma once
// esp32emu — WiFiUDP mock
// Simulates UDP communication using real host sockets (127.0.0.1).

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <deque>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include "WiFi.h"

class WiFiUDP {
public:
    WiFiUDP() = default;
    ~WiFiUDP() { stop(); }

    // Non-copyable
    WiFiUDP(const WiFiUDP&) = delete;
    WiFiUDP& operator=(const WiFiUDP&) = delete;

    uint8_t begin(uint16_t port) {
        stop();
        fd_ = socket(AF_INET, SOCK_DGRAM, 0);
        if (fd_ < 0) return 0;

        int opt = 1;
        setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        // Non-blocking
        fcntl(fd_, F_SETFL, O_NONBLOCK);

        struct sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = htons(port);

        if (bind(fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            close(fd_); fd_ = -1; return 0;
        }
        localPort_ = port;
        fprintf(stderr, "[esp32emu] WiFiUDP listening on 127.0.0.1:%d\n", port);
        return 1;
    }

    void stop() {
        if (fd_ >= 0) { close(fd_); fd_ = -1; }
        localPort_ = 0;
    }

    // --- Send API ---
    int beginPacket(const char* host, uint16_t port) {
        txBuf_.clear();
        destAddr_.sin_family = AF_INET;
        destAddr_.sin_port = htons(port);
        inet_pton(AF_INET, host, &destAddr_.sin_addr);
        return 1;
    }

    int beginPacket(IPAddress ip, uint16_t port) {
        char buf[20];
        snprintf(buf, sizeof(buf), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
        return beginPacket(buf, port);
    }

    size_t write(uint8_t byte) {
        txBuf_.push_back(byte);
        return 1;
    }

    size_t write(const uint8_t* buffer, size_t size) {
        txBuf_.insert(txBuf_.end(), buffer, buffer + size);
        return size;
    }

    size_t print(const char* str) {
        return write((const uint8_t*)str, strlen(str));
    }

    int endPacket() {
        if (fd_ < 0) return 0;
        ssize_t sent = sendto(fd_, txBuf_.data(), txBuf_.size(), 0,
                              (struct sockaddr*)&destAddr_, sizeof(destAddr_));
        txBuf_.clear();
        return sent > 0 ? 1 : 0;
    }

    // --- Receive API ---
    int parsePacket() {
        if (fd_ < 0) return 0;
        rxBuf_.resize(1500);
        socklen_t addrLen = sizeof(remoteAddr_);
        ssize_t n = recvfrom(fd_, rxBuf_.data(), rxBuf_.size(), 0,
                             (struct sockaddr*)&remoteAddr_, &addrLen);
        if (n <= 0) { rxBuf_.clear(); rxPos_ = 0; return 0; }
        rxBuf_.resize(n);
        rxPos_ = 0;
        return (int)n;
    }

    int available() {
        return (int)rxBuf_.size() - (int)rxPos_;
    }

    int read() {
        if (rxPos_ >= rxBuf_.size()) return -1;
        return rxBuf_[rxPos_++];
    }

    int read(uint8_t* buffer, size_t len) {
        size_t avail = rxBuf_.size() - rxPos_;
        size_t toRead = len < avail ? len : avail;
        memcpy(buffer, rxBuf_.data() + rxPos_, toRead);
        rxPos_ += toRead;
        return (int)toRead;
    }

    int read(char* buffer, size_t len) {
        return read((uint8_t*)buffer, len);
    }

    IPAddress remoteIP() {
        uint8_t* b = (uint8_t*)&remoteAddr_.sin_addr.s_addr;
        return IPAddress(b[0], b[1], b[2], b[3]);
    }

    uint16_t remotePort() {
        return ntohs(remoteAddr_.sin_port);
    }

    uint16_t localPort() const { return localPort_; }

    // Flush / peek
    int peek() {
        if (rxPos_ >= rxBuf_.size()) return -1;
        return rxBuf_[rxPos_];
    }
    void flush() {}

private:
    int fd_ = -1;
    uint16_t localPort_ = 0;
    struct sockaddr_in destAddr_{};
    struct sockaddr_in remoteAddr_{};
    std::vector<uint8_t> txBuf_;
    std::vector<uint8_t> rxBuf_;
    size_t rxPos_ = 0;
};
