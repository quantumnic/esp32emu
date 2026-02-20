#pragma once
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string>
#include <map>
#include <functional>
#include "esp_err.h"

// ESP-IDF Virtual File System mock
// Allows registering path prefixes for custom filesystem drivers

typedef int (*esp_vfs_write_t)(int fd, const void* data, size_t size);
typedef int (*esp_vfs_read_t)(int fd, void* dst, size_t size);
typedef int (*esp_vfs_open_t)(const char* path, int flags, int mode);
typedef int (*esp_vfs_close_t)(int fd);
typedef int (*esp_vfs_stat_t)(const char* path, struct stat* st);

typedef struct {
    int flags;
    esp_vfs_write_t write;
    esp_vfs_read_t  read;
    esp_vfs_open_t  open;
    esp_vfs_close_t close;
    esp_vfs_stat_t  stat;
} esp_vfs_t;

namespace esp32emu {
namespace vfs {

struct VfsEntry {
    std::string base_path;
    esp_vfs_t   vfs;
    void*       ctx;
};

inline std::map<std::string, VfsEntry>& registry() {
    static std::map<std::string, VfsEntry> r;
    return r;
}

inline void reset() { registry().clear(); }

} // namespace vfs
} // namespace esp32emu

inline esp_err_t esp_vfs_register(const char* base_path, const esp_vfs_t* vfs, void* ctx) {
    if (!base_path || !vfs) return ESP_ERR_INVALID_ARG;
    esp32emu::vfs::VfsEntry e;
    e.base_path = base_path;
    e.vfs = *vfs;
    e.ctx = ctx;
    esp32emu::vfs::registry()[base_path] = e;
    fprintf(stderr, "[esp32emu] VFS: registered '%s'\n", base_path);
    return ESP_OK;
}

inline esp_err_t esp_vfs_unregister(const char* base_path) {
    if (!base_path) return ESP_ERR_INVALID_ARG;
    auto& reg = esp32emu::vfs::registry();
    auto it = reg.find(base_path);
    if (it == reg.end()) return ESP_ERR_NOT_FOUND;
    reg.erase(it);
    fprintf(stderr, "[esp32emu] VFS: unregistered '%s'\n", base_path);
    return ESP_OK;
}

// Test helpers
namespace esp32emu {
namespace vfs {
inline bool isRegistered(const char* base_path) {
    return registry().count(base_path) > 0;
}
inline size_t count() { return registry().size(); }
} // namespace vfs
} // namespace esp32emu
