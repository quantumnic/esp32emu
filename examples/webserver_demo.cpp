// ESP32 WebServer demo — runs on localhost via esp32emu
#include "Arduino.h"
#include "WiFi.h"
#include "WebServer.h"

WebServer server(8080);

void handleRoot() {
    String html = "<html><body>";
    html += "<h1>ESP32 Emulator</h1>";
    html += "<p>Free Heap: " + String((int)ESP.getFreeHeap()) + " bytes</p>";
    html += "<p>Uptime: " + String((unsigned long)(millis() / 1000)) + "s</p>";
    html += "<p>GPIO 2: " + String(digitalRead(2)) + "</p>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleApi() {
    String json = "{";
    json += "\"heap\":" + String((int)ESP.getFreeHeap()) + ",";
    json += "\"uptime\":" + String((unsigned long)(millis() / 1000)) + ",";
    json += "\"chip\":\"" + String(ESP.getChipModel()) + "\"";
    json += "}";
    server.send(200, "application/json", json);
}

void handleNotFound() {
    server.send(404, "text/plain", "Not found: " + String(server.uri().c_str()));
}

void setup() {
    Serial.begin(115200);

    WiFi.begin("FakeNetwork", "password");
    Serial.println("WiFi connected: " + String(WiFi.localIP().toString().c_str()));

    server.on("/", handleRoot);
    server.on("/api/status", handleApi);
    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started on port 8080");
    Serial.println("Open http://127.0.0.1:8080 in your browser!");
}

void loop() {
    server.handleClient();
    delay(1);
}
