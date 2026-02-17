#include "Arduino.h"
#include "WiFi.h"
#include <cassert>
#include <cstdio>

int main() {
    assert(WiFi.status() == WL_DISCONNECTED);

    WiFi.begin("TestNetwork", "password123");
    assert(WiFi.status() == WL_CONNECTED);

    IPAddress ip = WiFi.localIP();
    assert(ip[0] == 127 && ip[3] == 1);

    String ssid = WiFi.SSID();
    assert(ssid == "TestNetwork");

    assert(WiFi.RSSI() < 0); // signal strength is negative

    String mac = WiFi.macAddress();
    assert(mac.length() > 0);

    WiFi.disconnect();
    assert(WiFi.status() == WL_DISCONNECTED);

    // SoftAP
    assert(WiFi.softAP("ESP32-AP"));
    IPAddress apIP = WiFi.softAPIP();
    assert(apIP[0] == 192);

    printf("test_wifi: all assertions passed\n");
    return 0;
}
