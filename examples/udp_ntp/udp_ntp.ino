// UDP NTP Client Example
// Sends an NTP request and parses the time response.
// In emulation, demonstrates WiFiUDP with real loopback sockets.

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUDP.h>

WiFiUDP udp;
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];

void sendNTPPacket(const char* server, uint16_t port) {
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    packetBuffer[0] = 0b11100011; // LI, Version, Mode
    packetBuffer[1] = 0;          // Stratum
    packetBuffer[2] = 6;          // Polling interval
    packetBuffer[3] = 0xEC;       // Peer clock precision

    udp.beginPacket(server, port);
    udp.write(packetBuffer, NTP_PACKET_SIZE);
    udp.endPacket();
    Serial.println("NTP request sent");
}

void setup() {
    Serial.begin(115200);
    Serial.println("UDP NTP Client Example");

    WiFi.begin("MyNetwork", "password");
    Serial.print("Connected, IP: ");
    Serial.println(WiFi.localIP().toString().c_str());

    udp.begin(2390);
    Serial.println("UDP started on port 2390");

    // In real use: sendNTPPacket("pool.ntp.org", 123);
    // For emulation demo, just show the API:
    sendNTPPacket("127.0.0.1", 123);
}

void loop() {
    int packetSize = udp.parsePacket();
    if (packetSize) {
        Serial.print("Received UDP packet, size: ");
        Serial.println(packetSize);

        udp.read(packetBuffer, NTP_PACKET_SIZE);

        // Extract NTP timestamp (bytes 40-43)
        unsigned long hi = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lo = word(packetBuffer[42], packetBuffer[43]);
        unsigned long secsSince1900 = hi << 16 | lo;
        unsigned long epoch = secsSince1900 - 2208988800UL;

        Serial.print("Unix time: ");
        Serial.println(epoch);
    }
    delay(1000);
}
