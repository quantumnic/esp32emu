// Ethernet Client — fetch data from a remote HTTP server
#include <Arduino.h>
#include <Ethernet.h>

byte mac[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
const char* serverHost = "httpbin.org";
const uint16_t serverPort = 80;

void setup() {
    Serial.begin(115200);
    Serial.println("Ethernet Client Example");

    if (Ethernet.begin(mac) == 0) {
        Serial.println("DHCP failed");
        while (true) delay(1000);
    }

    auto ip = Ethernet.localIP();
    char buf[20];
    snprintf(buf, sizeof(buf), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    Serial.print("Local IP: ");
    Serial.println(buf);
}

void loop() {
    EthernetClient client;

    Serial.print("Connecting to ");
    Serial.print(serverHost);
    Serial.println("...");

    if (client.connect(serverHost, serverPort)) {
        Serial.println("Connected!");
        client.println("GET /ip HTTP/1.1");
        client.print("Host: ");
        client.println(serverHost);
        client.println("Connection: close");
        client.println();

        while (client.connected()) {
            while (client.available()) {
                char c = client.read();
                Serial.print(c);
            }
            delay(10);
        }
        client.stop();
        Serial.println("\nDisconnected");
    } else {
        Serial.println("Connection failed");
    }

    delay(30000);
}
