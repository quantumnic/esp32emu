// TCP Echo Server using WiFiServer — echoes back whatever clients send
// Run with: esp32emu --board esp32
#include <WiFi.h>
#include <WiFiServer.h>

WiFiServer server(23); // Telnet port

void setup() {
    Serial.begin(115200);

    WiFi.begin("TestNetwork", "password123");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP().toString().c_str());

    server.begin();
    Serial.println("TCP Echo Server started on port 23");
    Serial.println("Connect with: telnet 127.0.0.1 23");
}

void loop() {
    WiFiClient client = server.available();
    if (client) {
        Serial.println("Client connected!");
        while (client.connected()) {
            if (client.available()) {
                String line = client.readStringUntil('\n');
                Serial.print("Received: ");
                Serial.println(line.c_str());

                // Echo back
                client.print("Echo: ");
                client.println(line.c_str());

                if (line.indexOf("quit") >= 0) {
                    client.println("Goodbye!");
                    break;
                }
            }
            delay(10);
        }
        client.stop();
        Serial.println("Client disconnected.");
    }
    delay(100);
}
