#pragma once
// esp32emu – ESP-IDF SPIFFS initialization mock
// Wraps the Arduino SPIFFS with esp_vfs_spiffs_register/unregister.

#include <cstdint>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *base_path;
    const char *partition_label;
    size_t max_files;
    bool format_if_mount_failed;
} esp_vfs_spiffs_conf_t;

// Track mount state
namespace esp32emu { namespace spiffs_state {
    inline bool mounted = false;
    inline std::string base_path;
    inline size_t total_bytes = 1048576;  // 1MB default
    inline size_t used_bytes = 0;
}}

inline esp_err_t esp_vfs_spiffs_register(const esp_vfs_spiffs_conf_t *conf) {
    if (!conf || !conf->base_path) return ESP_ERR_INVALID_ARG;
    if (esp32emu::spiffs_state::mounted) return ESP_ERR_INVALID_STATE;

    // Create the base directory on host filesystem
    std::string path = "/tmp/esp32emu_spiffs";
    if (conf->partition_label) {
        path += "_";
        path += conf->partition_label;
    }
    mkdir(path.c_str(), 0755);

    esp32emu::spiffs_state::mounted = true;
    esp32emu::spiffs_state::base_path = conf->base_path;
    return ESP_OK;
}

inline esp_err_t esp_vfs_spiffs_unregister(const char *partition_label) {
    (void)partition_label;
    esp32emu::spiffs_state::mounted = false;
    esp32emu::spiffs_state::base_path.clear();
    return ESP_OK;
}

inline esp_err_t esp_spiffs_info(const char *partition_label, size_t *total_bytes, size_t *used_bytes) {
    (void)partition_label;
    if (!esp32emu::spiffs_state::mounted) return ESP_ERR_INVALID_STATE;
    if (total_bytes) *total_bytes = esp32emu::spiffs_state::total_bytes;
    if (used_bytes) *used_bytes = esp32emu::spiffs_state::used_bytes;
    return ESP_OK;
}

inline bool esp_spiffs_mounted(const char *partition_label) {
    (void)partition_label;
    return esp32emu::spiffs_state::mounted;
}

inline esp_err_t esp_spiffs_format(const char *partition_label) {
    (void)partition_label;
    if (!esp32emu::spiffs_state::mounted) return ESP_ERR_INVALID_STATE;
    esp32emu::spiffs_state::used_bytes = 0;
    return ESP_OK;
}

#ifdef __cplusplus
}

// ── Test helpers ────────────────────────────────────────────────────
namespace esp32emu {
namespace spiffs {

inline void reset() {
    spiffs_state::mounted = false;
    spiffs_state::base_path.clear();
    spiffs_state::total_bytes = 1048576;
    spiffs_state::used_bytes = 0;
}

inline void set_usage(size_t total, size_t used) {
    spiffs_state::total_bytes = total;
    spiffs_state::used_bytes = used;
}

inline bool is_mounted() { return spiffs_state::mounted; }

} // namespace spiffs
} // namespace esp32emu
#endif
