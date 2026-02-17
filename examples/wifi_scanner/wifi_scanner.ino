// Fake WiFi Network Scanner
// esp32emu run examples/wifi_scanner/wifi_scanner.ino
#include "Arduino.h"
#include "WiFi.h"

struct FakeNetwork {
    const char* ssid;
    int rssi;
    int channel;
    const char* encryption;
};

FakeNetwork networks[] = {
    {"HomeNetwork_5G",    -42, 36, "WPA2"},
    {"FRITZ!Box 7590",    -55, 1,  "WPA2"},
    {"Vodafone-A1B2C3",   -63, 6,  "WPA2"},
    {"eduroam",           -71, 11, "WPA2-Enterprise"},
    {"FreeWifi_Airport",  -78, 3,  "Open"},
    {"TP-Link_Guest",     -80, 9,  "WPA"},
    {"IoT_Network",       -45, 44, "WPA3"},
    {"Hidden_Network",    -88, 1,  "WPA2"},
};
int networkCount = sizeof(networks) / sizeof(networks[0]);

void printBar(int rssi) {
    int bars = 0;
    if (rssi > -50) bars = 4;
    else if (rssi > -60) bars = 3;
    else if (rssi > -70) bars = 2;
    else if (rssi > -80) bars = 1;

    const char* color = bars >= 3 ? "\033[32m" : bars >= 2 ? "\033[33m" : "\033[31m";
    Serial.printf("%s", color);
    for (int i = 0; i < 4; i++) {
        Serial.print(i < bars ? "█" : "░");
    }
    Serial.print("\033[0m");
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    Serial.println("📡 WiFi Scanner\n");
}

void loop() {
    Serial.println("Scanning for networks...\n");
    delay(1000);

    Serial.println("┌─────────────────────────┬───────┬─────┬──────────────────┬──────┐");
    Serial.println("│ SSID                    │ RSSI  │ CH  │ Encryption       │ Sig  │");
    Serial.println("├─────────────────────────┼───────┼─────┼──────────────────┼──────┤");

    for (int i = 0; i < networkCount; i++) {
        // Add some RSSI variation
        int rssi = networks[i].rssi + (random(10) - 5);
        Serial.printf("│ %-23s │ %4d  │ %2d  │ %-16s │ ",
                      networks[i].ssid, rssi, networks[i].channel, networks[i].encryption);
        printBar(rssi);
        Serial.println(" │");
    }

    Serial.println("└─────────────────────────┴───────┴─────┴──────────────────┴──────┘");
    Serial.printf("\n📊 Found %d networks\n\n", networkCount);

    delay(5000);
}
