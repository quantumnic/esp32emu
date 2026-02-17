// WiFi Captive Portal with Config Page
// esp32emu run examples/captive_portal/captive_portal.ino
// Then open http://127.0.0.1:8080
#include "Arduino.h"
#include "WiFi.h"
#include "WebServer.h"

WebServer server(8080);

String savedSSID = "";
String savedPass = "";
bool configured = false;

const char* configPage = R"HTML(
<!DOCTYPE html>
<html><head>
<meta charset='utf-8'><meta name='viewport' content='width=device-width'>
<title>ESP32 WiFi Setup</title>
<style>
  body{font-family:sans-serif;max-width:400px;margin:40px auto;padding:20px;background:#1a1a2e;color:#eee}
  h1{color:#e94560;text-align:center}
  input,select{width:100%;padding:10px;margin:8px 0;border:1px solid #333;border-radius:5px;background:#16213e;color:#eee;box-sizing:border-box}
  button{width:100%;padding:12px;background:#e94560;color:#fff;border:none;border-radius:5px;cursor:pointer;font-size:16px}
  button:hover{background:#c73653}
  .net{padding:8px;border:1px solid #333;border-radius:5px;margin:4px 0;cursor:pointer;background:#16213e}
  .net:hover{background:#0f3460}
  .signal{float:right}
</style>
</head><body>
<h1>📡 WiFi Setup</h1>
<h3>Available Networks:</h3>
<div class='net' onclick="document.getElementById('s').value='HomeNetwork_5G'">HomeNetwork_5G <span class='signal'>████</span></div>
<div class='net' onclick="document.getElementById('s').value='FRITZ!Box 7590'">FRITZ!Box 7590 <span class='signal'>███░</span></div>
<div class='net' onclick="document.getElementById('s').value='eduroam'">eduroam <span class='signal'>██░░</span></div>
<form action='/save' method='GET'>
<input id='s' name='ssid' placeholder='WiFi SSID' required>
<input name='pass' type='password' placeholder='Password'>
<button type='submit'>💾 Save & Connect</button>
</form>
</body></html>
)HTML";

void handleRoot() {
    if (configured) {
        String html = "<html><body style='font-family:sans-serif;max-width:400px;margin:40px auto;padding:20px;background:#1a1a2e;color:#eee'>";
        html += "<h1 style='color:#4ecca3'>✅ Connected!</h1>";
        html += "<p>SSID: " + savedSSID + "</p>";
        html += "<p>IP: " + WiFi.localIP().toString() + "</p>";
        html += "<p>Uptime: " + String((unsigned long)(millis()/1000)) + "s</p>";
        html += "<a href='/reset' style='color:#e94560'>Reset Configuration</a>";
        html += "</body></html>";
        server.send(200, "text/html", html);
    } else {
        server.send(200, "text/html", configPage);
    }
}

void handleSave() {
    savedSSID = server.arg("ssid");
    savedPass = server.arg("pass");
    configured = true;

    Serial.printf("\033[32m[PORTAL] Saved config: SSID='%s'\033[0m\n", savedSSID.c_str());

    String html = "<html><body style='font-family:sans-serif;max-width:400px;margin:40px auto;padding:20px;background:#1a1a2e;color:#eee'>";
    html += "<h1 style='color:#4ecca3'>✅ Saved!</h1>";
    html += "<p>Connecting to: " + savedSSID + "</p>";
    html += "<meta http-equiv='refresh' content='2;url=/'>";
    html += "</body></html>";
    server.send(200, "text/html", html);
}

void handleReset() {
    configured = false;
    savedSSID = "";
    savedPass = "";
    Serial.println("\033[33m[PORTAL] Configuration reset\033[0m");
    server.sendHeader("Location", "/");
    server.send(302, "", "");
}

void setup() {
    Serial.begin(115200);
    
    // Start as Access Point
    WiFi.softAP("ESP32-Setup");
    
    Serial.println("📡 Captive Portal");
    Serial.printf("   AP Mode: SSID='ESP32-Setup'\n");
    Serial.printf("   Config page: http://127.0.0.1:8080\n\n");

    server.on("/", handleRoot);
    server.on("/save", handleSave);
    server.on("/reset", handleReset);
    server.begin();
}

void loop() {
    server.handleClient();
}
