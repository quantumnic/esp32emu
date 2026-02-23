// esp32emu example: AsyncUDP Broadcast Discovery
// Sends and receives UDP broadcast packets for device discovery.

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncUDP.h>

AsyncUDP udp;

void setup() {
    Serial.begin(115200);

    WiFi.begin("TestNetwork", "password123");
    while (WiFi.status() != WL_CONNECTED) delay(100);
    Serial.println("WiFi connected: " + WiFi.localIP().toString());

    if (udp.listen(9999)) {
        Serial.println("Listening on UDP port 9999");

        udp.onPacket([](AsyncUDPPacket& packet) {
            Serial.printf("Received %zu bytes from %s:%u\n",
                          packet.length(), packet.remoteIP(), packet.remotePort());

            char buf[256];
            size_t len = packet.length() < 255 ? packet.length() : 255;
            memcpy(buf, packet.data(), len);
            buf[len] = '\0';
            Serial.printf("Data: %s\n", buf);

            // Reply with device info
            packet.printf("ESP32-Device-%s", WiFi.localIP().toString().c_str());
        });
    }

    // Send discovery broadcast
    udp.broadcast((const uint8_t*)"DISCOVER", 8);
    Serial.println("Discovery broadcast sent");
}

void loop() {
    delay(5000);
    udp.write("heartbeat");
    Serial.println("Heartbeat sent");
}
