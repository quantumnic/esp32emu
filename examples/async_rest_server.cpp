// ESPAsyncWebServer REST API Example
// Demonstrates async HTTP server with JSON endpoints

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

float temperature = 22.5;
float humidity = 55.0;
bool ledState = false;

void setup() {
    Serial.begin(115200);
    Serial.println("Async REST API Server");
    Serial.println("=====================");

    WiFi.begin("MyNetwork", "password");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    // GET /api/status
    server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        char json[128];
        snprintf(json, sizeof(json),
                 "{\"temp\":%.1f,\"humidity\":%.1f,\"led\":%s,\"uptime\":%lu}",
                 temperature, humidity, ledState ? "true" : "false", millis() / 1000);
        request->send(200, "application/json", json);
    });

    // GET /api/led?state=on|off
    server.on("/api/led", HTTP_GET, [](AsyncWebServerRequest *request) {
        if (request->hasParam("state")) {
            std::string val = request->getParam("state");
            ledState = (val == "on" || val == "1");
            Serial.print("LED: ");
            Serial.println(ledState ? "ON" : "OFF");
            request->send(200, "application/json", ledState ? "{\"led\":true}" : "{\"led\":false}");
        } else {
            request->send(400, "application/json", "{\"error\":\"missing state param\"}");
        }
    });

    // Root
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html",
                      "<h1>ESP32 Async Server</h1>"
                      "<p><a href='/api/status'>Status</a></p>"
                      "<p><a href='/api/led?state=on'>LED On</a></p>");
    });

    server.begin();
    Serial.println("Server started on port 80");
}

void loop() {
    // Simulate sensor changes
    temperature += 0.1;
    if (temperature > 30.0) temperature = 20.0;
    delay(1000);
}
