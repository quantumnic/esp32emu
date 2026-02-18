// Test ESP-NOW mock
#include "Arduino.h"
#include "esp_now.h"
#include <cassert>
#include <cstring>
#include <cstdio>
#include <vector>

static std::vector<uint8_t> lastRecvData;
static uint8_t lastRecvMac[6];
static int sendStatus = -1;

void onRecv(const uint8_t* mac, const uint8_t* data, int len) {
    memcpy(lastRecvMac, mac, 6);
    lastRecvData.assign(data, data + len);
}

void onSend(const uint8_t* mac, uint8_t status) {
    (void)mac;
    sendStatus = status;
}

int main() {
    auto& espnow = esp32emu::ESPNow::instance();
    espnow.reset();

    // Init
    assert(esp_now_init() == 0);

    // Register callbacks
    esp_now_register_recv_cb(onRecv);
    esp_now_register_send_cb(onSend);

    // Add peer
    uint8_t peerMac[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    esp_now_peer_info_t peer{};
    memcpy(peer.peer_addr, peerMac, 6);
    peer.channel = 1;
    assert(esp_now_add_peer(&peer) == 0);
    assert(esp_now_is_peer_exist(peerMac) == 1);
    assert(espnow.peerCount() == 1);

    // Send data
    const uint8_t data[] = "Hello peer!";
    assert(esp_now_send(peerMac, data, sizeof(data)) == 0);
    assert(sendStatus == ESP_NOW_SEND_SUCCESS);

    auto log = espnow.getSentLog();
    assert(log.size() == 1);
    assert(memcmp(log[0].mac, peerMac, 6) == 0);
    assert(log[0].data.size() == sizeof(data));

    // Inject incoming message
    uint8_t srcMac[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    const uint8_t incoming[] = "Response!";
    espnow.injectRecv(srcMac, incoming, sizeof(incoming));
    assert(lastRecvData.size() == sizeof(incoming));
    assert(memcmp(lastRecvMac, srcMac, 6) == 0);
    assert(memcmp(lastRecvData.data(), incoming, sizeof(incoming)) == 0);

    // Delete peer
    assert(esp_now_del_peer(peerMac) == 0);
    assert(esp_now_is_peer_exist(peerMac) == 0);

    // Broadcast (null mac)
    espnow.clearSentLog();
    assert(esp_now_send(nullptr, data, sizeof(data)) == 0);
    auto log2 = espnow.getSentLog();
    assert(log2.size() == 1);
    // Broadcast mac should be FF:FF:FF:FF:FF:FF
    uint8_t bcast[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    assert(memcmp(log2[0].mac, bcast, 6) == 0);

    // Deinit
    assert(esp_now_deinit() == 0);

    printf("test_espnow: all assertions passed\n");
    return 0;
}
