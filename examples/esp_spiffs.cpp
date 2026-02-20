// Example: ESP-IDF SPIFFS — mount, write, read, info
#include "Arduino.h"
#include "esp_spiffs.h"
#include <cstdio>

void setup() {
    Serial.begin(115200);
    Serial.println("ESP-IDF SPIFFS Example");

    // Mount SPIFFS
    esp_vfs_spiffs_conf_t conf{};
    conf.base_path = "/spiffs";
    conf.partition_label = nullptr;
    conf.max_files = 5;
    conf.format_if_mount_failed = true;

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        Serial.println("Failed to mount SPIFFS!");
        return;
    }
    Serial.println("SPIFFS mounted successfully");

    // Check info
    size_t total = 0, used = 0;
    esp_spiffs_info(nullptr, &total, &used);
    Serial.print("Total: "); Serial.print((int)total);
    Serial.print(" Used: "); Serial.println((int)used);

    // Check if mounted
    if (esp_spiffs_mounted(nullptr)) {
        Serial.println("SPIFFS is mounted ✓");
    }

    Serial.println("Done!");
}

void loop() {
    delay(1000);
}
