#pragma once
// esp32emu: Ethernet library mock — Arduino-compatible Ethernet with EthernetClient/Server
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <functional>

#ifndef ESP32EMU_ETHERNET_NO_GLOBAL
#define ESP32EMU_ETHERNET_HAS_GLOBAL
#endif

class IPAddressEth {
public:
    IPAddressEth() { memset(octets_, 0, 4); }
    IPAddressEth(uint8_t a, uint8_t b, uint8_t c, uint8_t d) { octets_[0]=a; octets_[1]=b; octets_[2]=c; octets_[3]=d; }
    uint8_t operator[](int i) const { return octets_[i]; }
    operator uint32_t() const { return (octets_[3]<<24)|(octets_[2]<<16)|(octets_[1]<<8)|octets_[0]; }
    std::string toString() const {
        char buf[16]; snprintf(buf,sizeof(buf),"%d.%d.%d.%d",octets_[0],octets_[1],octets_[2],octets_[3]);
        return buf;
    }
private:
    uint8_t octets_[4];
};

class EthernetClient {
public:
    EthernetClient() = default;

    int connect(const char* host, uint16_t port) {
        host_ = host; port_ = port; connected_ = true;
        printf("[esp32emu] EthernetClient: connected to %s:%d\n", host, port);
        return 1;
    }
    int connect(IPAddressEth ip, uint16_t port) {
        char buf[16]; snprintf(buf,sizeof(buf),"%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]);
        return connect(buf, port);
    }

    bool connected() const { return connected_; }
    operator bool() const { return connected_; }
    void stop() { connected_ = false; printf("[esp32emu] EthernetClient: disconnected\n"); }

    size_t write(uint8_t b) { txBuf_.push_back(b); return 1; }
    size_t write(const uint8_t* buf, size_t len) { for(size_t i=0;i<len;i++) txBuf_.push_back(buf[i]); return len; }
    size_t print(const char* s) { return write(reinterpret_cast<const uint8_t*>(s), strlen(s)); }
    size_t println(const char* s) { size_t n=print(s); n+=write('\r'); n+=write('\n'); return n; }
    size_t println() { return print("\r\n"); }

    int available() const { return static_cast<int>(rxBuf_.size() - rxPos_); }
    int read() { if(rxPos_>=rxBuf_.size()) return -1; return rxBuf_[rxPos_++]; }
    int read(uint8_t* buf, size_t len) {
        int got=0; while(len-->0 && rxPos_<rxBuf_.size()) { buf[got++]=rxBuf_[rxPos_++]; } return got;
    }
    int peek() const { if(rxPos_>=rxBuf_.size()) return -1; return rxBuf_[rxPos_]; }
    void flush() { txBuf_.clear(); }

    // Test helpers
    void injectRx(const void* data, size_t len) {
        auto p = static_cast<const uint8_t*>(data);
        rxBuf_.insert(rxBuf_.end(), p, p+len);
    }
    void injectRx(const char* s) { injectRx(s, strlen(s)); }
    const std::vector<uint8_t>& getTxBuffer() const { return txBuf_; }
    std::string getTxString() const { return std::string(txBuf_.begin(), txBuf_.end()); }
    void clearTx() { txBuf_.clear(); }

private:
    bool connected_ = false;
    std::string host_;
    uint16_t port_ = 0;
    std::vector<uint8_t> txBuf_;
    std::vector<uint8_t> rxBuf_;
    size_t rxPos_ = 0;
};

class EthernetServer {
public:
    EthernetServer(uint16_t port) : port_(port) {}
    void begin() { running_ = true; printf("[esp32emu] EthernetServer: listening on port %d\n", port_); }
    EthernetClient available() {
        if (!pendingClients_.empty()) {
            auto c = pendingClients_.front();
            pendingClients_.erase(pendingClients_.begin());
            return c;
        }
        return EthernetClient();
    }
    uint16_t getPort() const { return port_; }
    bool isRunning() const { return running_; }

    // Test helper
    void injectClient(const EthernetClient& c) { pendingClients_.push_back(c); }

private:
    uint16_t port_;
    bool running_ = false;
    std::vector<EthernetClient> pendingClients_;
};

class EthernetClass {
public:
    int begin(uint8_t* mac, unsigned long timeout = 60000) {
        (void)timeout;
        memcpy(mac_, mac, 6);
        linkUp_ = true;
        printf("[esp32emu] Ethernet: initialized (MAC %02X:%02X:%02X:%02X:%02X:%02X) — DHCP\n",
               mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
        localIP_ = IPAddressEth(192,168,1,100);
        return 1;
    }

    void begin(uint8_t* mac, IPAddressEth ip) {
        memcpy(mac_, mac, 6);
        localIP_ = ip;
        linkUp_ = true;
        printf("[esp32emu] Ethernet: initialized (MAC %02X:%02X:%02X:%02X:%02X:%02X) — static %s\n",
               mac[0],mac[1],mac[2],mac[3],mac[4],mac[5], ip.toString().c_str());
    }

    void begin(uint8_t* mac, IPAddressEth ip, IPAddressEth dns) {
        (void)dns;
        begin(mac, ip);
    }

    void begin(uint8_t* mac, IPAddressEth ip, IPAddressEth dns, IPAddressEth gw) {
        (void)dns; (void)gw;
        begin(mac, ip);
    }

    void begin(uint8_t* mac, IPAddressEth ip, IPAddressEth dns, IPAddressEth gw, IPAddressEth subnet) {
        (void)dns; (void)gw; (void)subnet;
        begin(mac, ip);
    }

    IPAddressEth localIP() const { return localIP_; }
    int linkStatus() const { return linkUp_ ? 1 : 0; } // LinkON=1
    int hardwareStatus() const { return 1; } // EthernetW5500=1

    void setLinkUp(bool up) { linkUp_ = up; }
    void setLocalIP(IPAddressEth ip) { localIP_ = ip; }
    bool isInitialized() const { return linkUp_; }

    // MACAddress helper
    void MACAddress(uint8_t* out) const { memcpy(out, mac_, 6); }

private:
    bool linkUp_ = false;
    uint8_t mac_[6] = {};
    IPAddressEth localIP_;
};

#ifdef ESP32EMU_ETHERNET_HAS_GLOBAL
inline EthernetClass Ethernet;
#endif
