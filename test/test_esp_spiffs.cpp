// esp32emu – ESP-IDF SPIFFS API tests
#include "esp_spiffs.h"
#include <cassert>
#include <cstdio>

int main() {
    esp32emu::spiffs::reset();

    // Not mounted initially
    assert(!esp_spiffs_mounted(nullptr));

    // Register
    esp_vfs_spiffs_conf_t conf{};
    conf.base_path = "/spiffs";
    conf.partition_label = "storage";
    conf.max_files = 5;
    conf.format_if_mount_failed = true;
    assert(esp_vfs_spiffs_register(&conf) == ESP_OK);
    assert(esp_spiffs_mounted(nullptr));

    // Double mount fails
    assert(esp_vfs_spiffs_register(&conf) == ESP_ERR_INVALID_STATE);

    // Info
    size_t total = 0, used = 0;
    assert(esp_spiffs_info(nullptr, &total, &used) == ESP_OK);
    assert(total == 1048576);
    assert(used == 0);

    // Set usage
    esp32emu::spiffs::set_usage(2097152, 512000);
    esp_spiffs_info(nullptr, &total, &used);
    assert(total == 2097152);
    assert(used == 512000);

    // Format
    assert(esp_spiffs_format(nullptr) == ESP_OK);
    esp_spiffs_info(nullptr, &total, &used);
    assert(used == 0);

    // Unregister
    assert(esp_vfs_spiffs_unregister(nullptr) == ESP_OK);
    assert(!esp_spiffs_mounted(nullptr));

    // Info fails when unmounted
    assert(esp_spiffs_info(nullptr, &total, &used) == ESP_ERR_INVALID_STATE);

    // Invalid args
    assert(esp_vfs_spiffs_register(nullptr) == ESP_ERR_INVALID_ARG);

    printf("test_esp_spiffs: all assertions passed\n");
    return 0;
}
