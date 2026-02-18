#pragma once
// esp32emu — ESP-NOW mock
// Simulates ESP-NOW peer-to-peer communication for testing.
// Messages stay in-process; use injectRecv() to simulate incoming data.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <vector>
#include <mutex>

#define ESP_NOW_MAX_DATA_LEN 250
#define ESP_NOW_ETH_ALEN 6
#define ESP_NOW_KEY_LEN 16

#define ESP_NOW_SEND_SUCCESS 0
#define ESP_NOW_SEND_FAIL    1

typedef enum {
    ESP_NOW_ROLE_IDLE = 0,
    ESP_NOW_ROLE_CONTROLLER = 1,
    ESP_NOW_ROLE_SLAVE = 2,
    ESP_NOW_ROLE_COMBO = 3
} esp_now_role_t;

typedef struct {
    uint8_t peer_addr[ESP_NOW_ETH_ALEN];
    uint8_t lmk[ESP_NOW_KEY_LEN];
    uint8_t channel;
    bool encrypt;
    esp_now_role_t role;
} esp_now_peer_info_t;

typedef void (*esp_now_send_cb_t)(const uint8_t* mac_addr, uint8_t status);
typedef void (*esp_now_recv_cb_t)(const uint8_t* mac_addr, const uint8_t* data, int data_len);

namespace esp32emu {

class ESPNow {
public:
    static ESPNow& instance() {
        static ESPNow inst;
        return inst;
    }

    int init() {
        initialized_ = true;
        fprintf(stderr, "[esp32emu] ESP-NOW initialized\n");
        return 0; // ESP_OK
    }

    int deinit() {
        initialized_ = false;
        peers_.clear();
        sendCb_ = nullptr;
        recvCb_ = nullptr;
        sentLog_.clear();
        fprintf(stderr, "[esp32emu] ESP-NOW deinitialized\n");
        return 0;
    }

    int registerSendCb(esp_now_send_cb_t cb) { sendCb_ = cb; return 0; }
    int registerRecvCb(esp_now_recv_cb_t cb) { recvCb_ = cb; return 0; }

    int addPeer(const esp_now_peer_info_t* peer) {
        std::lock_guard<std::mutex> lk(mtx_);
        peers_.push_back(*peer);
        fprintf(stderr, "[esp32emu] ESP-NOW peer added: %02X:%02X:%02X:%02X:%02X:%02X\n",
                peer->peer_addr[0], peer->peer_addr[1], peer->peer_addr[2],
                peer->peer_addr[3], peer->peer_addr[4], peer->peer_addr[5]);
        return 0;
    }

    int delPeer(const uint8_t* mac) {
        std::lock_guard<std::mutex> lk(mtx_);
        peers_.erase(std::remove_if(peers_.begin(), peers_.end(),
            [mac](const esp_now_peer_info_t& p) {
                return memcmp(p.peer_addr, mac, ESP_NOW_ETH_ALEN) == 0;
            }), peers_.end());
        return 0;
    }

    bool isPeerExist(const uint8_t* mac) {
        std::lock_guard<std::mutex> lk(mtx_);
        for (auto& p : peers_)
            if (memcmp(p.peer_addr, mac, ESP_NOW_ETH_ALEN) == 0) return true;
        return false;
    }

    int send(const uint8_t* peer_addr, const uint8_t* data, size_t len) {
        if (!initialized_) return -1;
        std::lock_guard<std::mutex> lk(mtx_);
        SentMessage msg;
        if (peer_addr) memcpy(msg.mac, peer_addr, 6); else memset(msg.mac, 0xFF, 6);
        msg.data.assign(data, data + len);
        sentLog_.push_back(msg);
        fprintf(stderr, "[esp32emu] ESP-NOW send %zu bytes\n", len);
        if (sendCb_) sendCb_(peer_addr, ESP_NOW_SEND_SUCCESS);
        return 0;
    }

    // ── Test helpers ──
    struct SentMessage {
        uint8_t mac[6];
        std::vector<uint8_t> data;
    };

    // Inject a received message (simulates incoming ESP-NOW data)
    void injectRecv(const uint8_t* mac, const uint8_t* data, int len) {
        if (recvCb_) recvCb_(mac, data, len);
    }

    std::vector<SentMessage> getSentLog() const {
        std::lock_guard<std::mutex> lk(mtx_);
        return sentLog_;
    }

    void clearSentLog() {
        std::lock_guard<std::mutex> lk(mtx_);
        sentLog_.clear();
    }

    size_t peerCount() const {
        std::lock_guard<std::mutex> lk(mtx_);
        return peers_.size();
    }

    void reset() {
        std::lock_guard<std::mutex> lk(mtx_);
        initialized_ = false;
        peers_.clear();
        sendCb_ = nullptr;
        recvCb_ = nullptr;
        sentLog_.clear();
    }

private:
    bool initialized_ = false;
    esp_now_send_cb_t sendCb_ = nullptr;
    esp_now_recv_cb_t recvCb_ = nullptr;
    std::vector<esp_now_peer_info_t> peers_;
    std::vector<SentMessage> sentLog_;
    mutable std::mutex mtx_;
};

} // namespace esp32emu

// ── Arduino-style free functions ──
inline int esp_now_init() { return esp32emu::ESPNow::instance().init(); }
inline int esp_now_deinit() { return esp32emu::ESPNow::instance().deinit(); }
inline int esp_now_register_send_cb(esp_now_send_cb_t cb) { return esp32emu::ESPNow::instance().registerSendCb(cb); }
inline int esp_now_register_recv_cb(esp_now_recv_cb_t cb) { return esp32emu::ESPNow::instance().registerRecvCb(cb); }
inline int esp_now_add_peer(const esp_now_peer_info_t* peer) { return esp32emu::ESPNow::instance().addPeer(peer); }
inline int esp_now_del_peer(const uint8_t* mac) { return esp32emu::ESPNow::instance().delPeer(mac); }
inline int esp_now_is_peer_exist(const uint8_t* mac) { return esp32emu::ESPNow::instance().isPeerExist(mac) ? 1 : 0; }
inline int esp_now_send(const uint8_t* peer_addr, const uint8_t* data, size_t len) {
    return esp32emu::ESPNow::instance().send(peer_addr, data, len);
}
