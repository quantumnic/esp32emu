// Ethernet Web Server — serve sensor data over wired Ethernet
#include <Arduino.h>
#include <Ethernet.h>

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetServer server(80);
int requestCount = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing Ethernet...");

    if (Ethernet.begin(mac) == 0) {
        Serial.println("Failed to configure Ethernet using DHCP");
        while (true) delay(1000);
    }

    Serial.print("IP Address: ");
    auto ip = Ethernet.localIP();
    char buf[20];
    snprintf(buf, sizeof(buf), "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    Serial.println(buf);

    server.begin();
    Serial.println("Ethernet Web Server started");
}

void loop() {
    EthernetClient client = server.available();
    if (client) {
        requestCount++;
        Serial.print("Client connected (#");
        Serial.print(requestCount);
        Serial.println(")");

        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        client.println("<html><body>");
        client.print("<h1>ESP32 Ethernet Server</h1>");
        client.print("<p>Requests: ");
        char num[16];
        snprintf(num, sizeof(num), "%d", requestCount);
        client.print(num);
        client.println("</p>");
        client.print("<p>Uptime: ");
        snprintf(num, sizeof(num), "%lu", millis() / 1000);
        client.print(num);
        client.println("s</p>");
        client.println("</body></html>");
        client.stop();
    }
    delay(10);
}
