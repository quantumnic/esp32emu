#pragma once
// esp32emu mock: ESP-IDF wear levelling API
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <map>
#include <vector>

typedef int32_t esp_err_t;
#ifndef ESP_OK
#define ESP_OK 0
#endif
#ifndef ESP_FAIL
#define ESP_FAIL -1
#endif
#define ESP_ERR_INVALID_ARG 0x102
#define ESP_ERR_NO_MEM      0x101

typedef int32_t wl_handle_t;
#define WL_INVALID_HANDLE (-1)

// Simulated flash partition
struct _wl_partition {
    std::vector<uint8_t> data;
    size_t sector_size;
    size_t size;
};

inline std::map<wl_handle_t, _wl_partition>& _wl_partitions() {
    static std::map<wl_handle_t, _wl_partition> p;
    return p;
}
inline wl_handle_t _wl_next_handle() {
    static wl_handle_t h = 0;
    return h++;
}

// Mount wear levelling on a partition (mock: creates memory-backed storage)
inline esp_err_t wl_mount(const void* partition, wl_handle_t* out_handle,
                           size_t size = 4096 * 16, size_t sector_size = 4096) {
    (void)partition;
    if (!out_handle) return ESP_ERR_INVALID_ARG;
    wl_handle_t h = _wl_next_handle();
    _wl_partitions()[h] = {std::vector<uint8_t>(size, 0xFF), sector_size, size};
    *out_handle = h;
    printf("[esp32emu] WearLevelling: mounted handle=%d size=%zu\n", h, size);
    return ESP_OK;
}

inline esp_err_t wl_unmount(wl_handle_t handle) {
    _wl_partitions().erase(handle);
    return ESP_OK;
}

inline esp_err_t wl_read(wl_handle_t handle, size_t addr, void* dest, size_t size) {
    auto it = _wl_partitions().find(handle);
    if (it == _wl_partitions().end()) return ESP_FAIL;
    if (addr + size > it->second.size) return ESP_ERR_INVALID_ARG;
    memcpy(dest, it->second.data.data() + addr, size);
    return ESP_OK;
}

inline esp_err_t wl_write(wl_handle_t handle, size_t addr, const void* src, size_t size) {
    auto it = _wl_partitions().find(handle);
    if (it == _wl_partitions().end()) return ESP_FAIL;
    if (addr + size > it->second.size) return ESP_ERR_INVALID_ARG;
    memcpy(it->second.data.data() + addr, src, size);
    return ESP_OK;
}

inline esp_err_t wl_erase_range(wl_handle_t handle, size_t addr, size_t size) {
    auto it = _wl_partitions().find(handle);
    if (it == _wl_partitions().end()) return ESP_FAIL;
    if (addr + size > it->second.size) return ESP_ERR_INVALID_ARG;
    memset(it->second.data.data() + addr, 0xFF, size);
    return ESP_OK;
}

inline size_t wl_size(wl_handle_t handle) {
    auto it = _wl_partitions().find(handle);
    if (it == _wl_partitions().end()) return 0;
    return it->second.size;
}

inline size_t wl_sector_size(wl_handle_t handle) {
    auto it = _wl_partitions().find(handle);
    if (it == _wl_partitions().end()) return 0;
    return it->second.sector_size;
}
