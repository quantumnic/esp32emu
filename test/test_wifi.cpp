#include "Arduino.h"
#include "WiFi.h"
#include <cassert>
#include <cstdio>
#include <vector>
#include <string>

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

    // WiFi scan API
    {
        std::vector<WiFiClass::ScanResult> nets = {
            {"HomeNet", -40},
            {"CafeGuest", -67},
            {"Neighbor", -80},
        };
        WiFi.test_setScanResults(nets);
        int n = WiFi.scanNetworks();
        assert(n == 3);
        assert(std::string(WiFi.SSID(0).c_str()) == "HomeNet");
        assert(WiFi.RSSI(1) == -67);
        assert(WiFi.RSSI(9) == 0);
        assert(std::string(WiFi.SSID(9).c_str()) == "");

        WiFi.setWiFiCheck([]() { return false; });
        assert(WiFi.scanNetworks() == 0);
        WiFi.setWiFiCheck([]() { return true; });
        WiFi.test_setScanResults({});
    }

    // WiFiClient usable via WiFi.h include
    {
        WiFiClient c;
        assert(!c.connected());
    }

    printf("test_wifi: all assertions passed\n");
    return 0;
}
