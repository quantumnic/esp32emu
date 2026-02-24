// esp32emu example: mDNS service discovery
// Registers a web server via mDNS and discovers other services on the network.

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>

WebServer server(80);

void handleRoot() {
    server.send(200, "text/html",
        "<h1>ESP32 mDNS Demo</h1>"
        "<p>Find me at <b>esp32-node.local</b></p>");
}

void setup() {
    Serial.begin(115200);
    WiFi.begin("MyNetwork", "password");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.printf("\nWiFi connected: %s\n", WiFi.localIP().toString().c_str());

    // Register mDNS hostname
    if (MDNS.begin("esp32-node")) {
        Serial.println("mDNS: esp32-node.local registered");
    }

    // Advertise HTTP service with metadata
    MDNS.addService("http", "tcp", 80);
    MDNS.addServiceTxt("http", "tcp", "board", "ESP32");
    MDNS.addServiceTxt("http", "tcp", "version", "1.0.0");

    // Discover other HTTP services
    Serial.println("\nDiscovering HTTP services...");
    int n = MDNS.queryService("http", "tcp");
    if (n == 0) {
        Serial.println("No services found");
    } else {
        Serial.printf("Found %d service(s):\n", n);
        for (int i = 0; i < n; i++) {
            Serial.printf("  %d) %s : %d\n", i + 1,
                MDNS.hostname(i).c_str(), MDNS.port(i));
        }
    }

    // Start web server
    server.on("/", handleRoot);
    server.begin();
    Serial.println("HTTP server started on port 80");
}

void loop() {
    server.handleClient();
}
