// ESP-NOW Sender Example
// Sends sensor data to a peer device using ESP-NOW protocol.
// In emulation, demonstrates the ESP-NOW API with mock callbacks.

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// Peer MAC address
uint8_t peerMAC[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

// Data structure (must match receiver)
typedef struct {
    float temperature;
    float humidity;
    uint32_t counter;
} SensorData;

SensorData outgoing;
uint32_t counter = 0;

void onDataSent(const uint8_t* mac, uint8_t status) {
    Serial.print("Send status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}

void onDataRecv(const uint8_t* mac, const uint8_t* data, int len) {
    Serial.print("Received ");
    Serial.print(len);
    Serial.print(" bytes from ");
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.println(macStr);
}

void setup() {
    Serial.begin(115200);
    Serial.println("ESP-NOW Sender Example");

    WiFi.mode(WIFI_STA);
    WiFi.begin("network", "pass");

    // Init ESP-NOW
    if (esp_now_init() != 0) {
        Serial.println("ESP-NOW init failed!");
        return;
    }

    esp_now_register_send_cb(onDataSent);
    esp_now_register_recv_cb(onDataRecv);

    // Add peer
    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, peerMAC, 6);
    peer.channel = 0;
    peer.encrypt = false;
    esp_now_add_peer(&peer);

    Serial.println("ESP-NOW ready, sending data...");
}

void loop() {
    // Simulate sensor readings
    outgoing.temperature = 22.5 + (counter % 10) * 0.3;
    outgoing.humidity = 45.0 + (counter % 20) * 0.5;
    outgoing.counter = counter++;

    Serial.print("Sending: T=");
    Serial.print(outgoing.temperature);
    Serial.print("°C H=");
    Serial.print(outgoing.humidity);
    Serial.print("% #");
    Serial.println(outgoing.counter);

    esp_now_send(peerMAC, (uint8_t*)&outgoing, sizeof(outgoing));

    delay(2000);
}
