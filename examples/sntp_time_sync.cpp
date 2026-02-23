#include <Arduino.h>
#include <WiFi.h>
#include "esp_sntp.h"

void timeSyncCallback(struct timeval* tv) {
    Serial.println("Time synchronized via SNTP!");
    (void)tv;
}

void setup() {
    Serial.begin(115200);
    WiFi.begin("MyNetwork", "password");
    Serial.println("Connecting to WiFi...");

    // Configure SNTP
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_setservername(1, "time.google.com");
    esp32emu_sntp_set_sync_callback(timeSyncCallback);
    sntp_init();

    Serial.print("SNTP status: ");
    Serial.println(sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED
                   ? "synchronized" : "pending");
}

void loop() {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    Serial.print("Current time: ");
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
    Serial.println(buf);
    delay(10000);
}
