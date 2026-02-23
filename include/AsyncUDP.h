#pragma once
// esp32emu — AsyncUDP mock
// Simulates ESP32 AsyncUDP for event-driven UDP communication.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <vector>
#include <string>

struct AsyncUDPPacket;
using AsyncUDPPacketHandler = std::function<void(AsyncUDPPacket&)>;

struct IPAddress_t {
    uint8_t octets[4] = {0, 0, 0, 0};
    IPAddress_t() = default;
    IPAddress_t(uint8_t a, uint8_t b, uint8_t c, uint8_t d) { octets[0]=a; octets[1]=b; octets[2]=c; octets[3]=d; }
};

struct AsyncUDPPacket {
    uint8_t* data() { return data_.data(); }
    size_t length() const { return data_.size(); }
    const char* remoteIP() const { return remote_ip_.c_str(); }
    uint16_t remotePort() const { return remote_port_; }
    const char* localIP() const { return "0.0.0.0"; }
    uint16_t localPort() const { return local_port_; }
    bool isBroadcast() const { return broadcast_; }
    bool isMulticast() const { return multicast_; }

    size_t write(const uint8_t* buf, size_t len) {
        reply_data_.assign(buf, buf + len);
        return len;
    }
    size_t write(const char* s) { return write((const uint8_t*)s, strlen(s)); }
    size_t printf(const char* fmt, ...) __attribute__((format(printf, 2, 3))) {
        char buf[256];
        va_list args;
        va_start(args, fmt);
        int n = vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        if (n > 0) write((const uint8_t*)buf, n);
        return n > 0 ? n : 0;
    }

    // Test helpers
    std::vector<uint8_t> data_;
    std::string remote_ip_ = "192.168.1.100";
    uint16_t remote_port_ = 12345;
    uint16_t local_port_ = 0;
    bool broadcast_ = false;
    bool multicast_ = false;
    std::vector<uint8_t> reply_data_;
};

class AsyncUDP {
public:
    AsyncUDP() = default;
    ~AsyncUDP() { close(); }

    bool listen(uint16_t port) {
        port_ = port;
        listening_ = true;
        fprintf(stderr, "[esp32emu] AsyncUDP: listening on port %u\n", port);
        return true;
    }

    bool listen(const IPAddress_t& addr, uint16_t port) {
        (void)addr;
        return listen(port);
    }

    bool listenMulticast(const IPAddress_t& addr, uint16_t port) {
        (void)addr;
        multicast_ = true;
        return listen(port);
    }

    void close() {
        listening_ = false;
        connected_ = false;
    }

    bool connect(const IPAddress_t& ip, uint16_t port) {
        (void)ip;
        remote_port_ = port;
        connected_ = true;
        return true;
    }

    size_t write(const uint8_t* data, size_t len) {
        sent_data_.assign(data, data + len);
        total_sent_ += len;
        return len;
    }

    size_t write(const char* s) { return write((const uint8_t*)s, strlen(s)); }

    size_t writeTo(const uint8_t* data, size_t len, const IPAddress_t& ip, uint16_t port) {
        (void)ip; (void)port;
        return write(data, len);
    }

    size_t broadcastTo(const uint8_t* data, size_t len, uint16_t port) {
        (void)port;
        return write(data, len);
    }

    size_t broadcast(const uint8_t* data, size_t len) {
        return write(data, len);
    }

    void onPacket(AsyncUDPPacketHandler handler) { handler_ = handler; }

    // ── Test helpers ──
    bool isListening() const { return listening_; }
    bool isConnected() const { return connected_; }
    bool isMulticast() const { return multicast_; }
    uint16_t getPort() const { return port_; }
    size_t getTotalSent() const { return total_sent_; }
    const std::vector<uint8_t>& getSentData() const { return sent_data_; }

    // Inject a received packet for testing
    void injectPacket(const uint8_t* data, size_t len,
                      const char* remoteIP = "192.168.1.100", uint16_t remotePort = 12345) {
        if (handler_) {
            AsyncUDPPacket pkt;
            pkt.data_.assign(data, data + len);
            pkt.remote_ip_ = remoteIP;
            pkt.remote_port_ = remotePort;
            pkt.local_port_ = port_;
            handler_(pkt);
        }
    }

    void injectPacket(const char* s, const char* remoteIP = "192.168.1.100", uint16_t remotePort = 12345) {
        injectPacket((const uint8_t*)s, strlen(s), remoteIP, remotePort);
    }

private:
    uint16_t port_ = 0;
    uint16_t remote_port_ = 0;
    bool listening_ = false;
    bool connected_ = false;
    bool multicast_ = false;
    size_t total_sent_ = 0;
    std::vector<uint8_t> sent_data_;
    AsyncUDPPacketHandler handler_;
};
