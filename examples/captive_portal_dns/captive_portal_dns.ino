// Captive Portal with DNSServer — redirect all DNS to our web server
// Run with: esp32emu --board esp32
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

DNSServer dnsServer;
WebServer server(80);

const char* ssid = "ESP32-CaptivePortal";

void handleRoot() {
    String html = "<html><head><title>Captive Portal</title>"
                  "<style>body{font-family:Arial;text-align:center;padding:40px;}"
                  "h1{color:#333;}input,button{padding:10px;margin:5px;font-size:16px;}"
                  "button{background:#4CAF50;color:white;border:none;cursor:pointer;}</style></head>"
                  "<body><h1>WiFi Setup</h1>"
                  "<p>Configure your device's WiFi connection:</p>"
                  "<form action='/save' method='POST'>"
                  "<input type='text' name='ssid' placeholder='WiFi SSID'><br>"
                  "<input type='password' name='pass' placeholder='Password'><br>"
                  "<button type='submit'>Connect</button>"
                  "</form></body></html>";
    server.send(200, "text/html", html.c_str());
}

void handleSave() {
    String ssid = server.arg("ssid");
    String pass = server.arg("pass");
    Serial.print("Received WiFi config — SSID: ");
    Serial.println(ssid.c_str());
    server.send(200, "text/html",
        "<html><body><h1>Saved!</h1><p>Device will restart and connect.</p></body></html>");
}

void handleNotFound() {
    // Redirect all unknown URLs to the portal
    server.sendHeader("Location", "http://192.168.4.1/");
    server.send(302, "text/plain", "Redirecting to captive portal...");
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting Captive Portal...");

    WiFi.softAP(ssid);
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP().toString().c_str());

    // Start DNS server — redirect all domains to our IP
    dnsServer.start(53, "*", WiFi.softAPIP());

    server.on("/", handleRoot);
    server.on("/save", HTTP_POST, handleSave);
    server.onNotFound(handleNotFound);
    server.begin();

    Serial.println("Captive portal ready — connect to " + String(ssid));
}

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();
}
