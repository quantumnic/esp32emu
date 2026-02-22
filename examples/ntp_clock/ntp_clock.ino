// NTP Clock — synchronize time and display a digital clock
#include <WiFi.h>

void setup() {
    Serial.begin(115200);
    WiFi.begin("MySSID", "MyPassword");
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" connected!");

    // Configure NTP: GMT+1, 1h DST, pool.ntp.org
    configTime(3600, 3600, "pool.ntp.org", "time.nist.gov");
    Serial.println("⏰ NTP time configured");

    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        char buf[64];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
        Serial.print("Current time: ");
        Serial.println(buf);
    }
}

void loop() {
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
        char buf[32];
        strftime(buf, sizeof(buf), "%H:%M:%S", &timeinfo);
        Serial.print("🕐 ");
        Serial.println(buf);
    }
    delay(1000);
}
