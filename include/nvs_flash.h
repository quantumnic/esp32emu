#pragma once
// esp32emu — NVS Flash mock
// Provides nvs_flash_init, nvs_flash_erase, and the nvs_handle API.

#include <cstdint>
#include <cstring>
#include <map>
#include <string>
#include <mutex>

#define ESP_OK 0
#define ESP_FAIL -1
#define ESP_ERR_NVS_NOT_FOUND      0x1102
#define ESP_ERR_NVS_INVALID_HANDLE 0x1109
#define ESP_ERR_NVS_INVALID_NAME   0x110B

typedef int esp_err_t;
typedef uint32_t nvs_handle_t;

typedef enum {
    NVS_READONLY  = 0,
    NVS_READWRITE = 1
} nvs_open_mode_t;

namespace esp32emu {
namespace nvs {

struct NVSStore {
    std::mutex mtx;
    // namespace -> key -> blob
    std::map<std::string, std::map<std::string, std::string>> data;
    uint32_t next_handle = 1;
    std::map<uint32_t, std::pair<std::string, nvs_open_mode_t>> handles;
    bool initialized = false;

    static NVSStore& instance() {
        static NVSStore s;
        return s;
    }

    void clear() {
        std::lock_guard<std::mutex> lk(mtx);
        data.clear();
        handles.clear();
        next_handle = 1;
        initialized = false;
    }
};

} // namespace nvs
} // namespace esp32emu

inline esp_err_t nvs_flash_init() {
    auto& s = esp32emu::nvs::NVSStore::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    s.initialized = true;
    return ESP_OK;
}

inline esp_err_t nvs_flash_erase() {
    auto& s = esp32emu::nvs::NVSStore::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    s.data.clear();
    return ESP_OK;
}

inline esp_err_t nvs_open(const char* ns, nvs_open_mode_t mode, nvs_handle_t* out) {
    if (!ns || !out) return ESP_ERR_NVS_INVALID_NAME;
    auto& s = esp32emu::nvs::NVSStore::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    uint32_t h = s.next_handle++;
    s.handles[h] = {std::string(ns), mode};
    *out = h;
    return ESP_OK;
}

inline esp_err_t nvs_close(nvs_handle_t h) {
    auto& s = esp32emu::nvs::NVSStore::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    s.handles.erase(h);
    return ESP_OK;
}

// Store value as raw bytes in string
inline esp_err_t nvs_set_i32(nvs_handle_t h, const char* key, int32_t value) {
    auto& s = esp32emu::nvs::NVSStore::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    auto it = s.handles.find(h);
    if (it == s.handles.end()) return ESP_ERR_NVS_INVALID_HANDLE;
    if (it->second.second != NVS_READWRITE) return ESP_FAIL;
    std::string blob(reinterpret_cast<const char*>(&value), sizeof(value));
    s.data[it->second.first][key] = blob;
    return ESP_OK;
}

inline esp_err_t nvs_get_i32(nvs_handle_t h, const char* key, int32_t* out) {
    auto& s = esp32emu::nvs::NVSStore::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    auto it = s.handles.find(h);
    if (it == s.handles.end()) return ESP_ERR_NVS_INVALID_HANDLE;
    auto& ns_data = s.data[it->second.first];
    auto kit = ns_data.find(key);
    if (kit == ns_data.end()) return ESP_ERR_NVS_NOT_FOUND;
    if (kit->second.size() != sizeof(int32_t)) return ESP_FAIL;
    std::memcpy(out, kit->second.data(), sizeof(int32_t));
    return ESP_OK;
}

inline esp_err_t nvs_set_u32(nvs_handle_t h, const char* key, uint32_t value) {
    auto& s = esp32emu::nvs::NVSStore::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    auto it = s.handles.find(h);
    if (it == s.handles.end()) return ESP_ERR_NVS_INVALID_HANDLE;
    if (it->second.second != NVS_READWRITE) return ESP_FAIL;
    std::string blob(reinterpret_cast<const char*>(&value), sizeof(value));
    s.data[it->second.first][key] = blob;
    return ESP_OK;
}

inline esp_err_t nvs_get_u32(nvs_handle_t h, const char* key, uint32_t* out) {
    auto& s = esp32emu::nvs::NVSStore::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    auto it = s.handles.find(h);
    if (it == s.handles.end()) return ESP_ERR_NVS_INVALID_HANDLE;
    auto& ns_data = s.data[it->second.first];
    auto kit = ns_data.find(key);
    if (kit == ns_data.end()) return ESP_ERR_NVS_NOT_FOUND;
    if (kit->second.size() != sizeof(uint32_t)) return ESP_FAIL;
    std::memcpy(out, kit->second.data(), sizeof(uint32_t));
    return ESP_OK;
}

inline esp_err_t nvs_set_str(nvs_handle_t h, const char* key, const char* value) {
    auto& s = esp32emu::nvs::NVSStore::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    auto it = s.handles.find(h);
    if (it == s.handles.end()) return ESP_ERR_NVS_INVALID_HANDLE;
    if (it->second.second != NVS_READWRITE) return ESP_FAIL;
    s.data[it->second.first][key] = std::string(value);
    return ESP_OK;
}

inline esp_err_t nvs_get_str(nvs_handle_t h, const char* key, char* out, size_t* len) {
    auto& s = esp32emu::nvs::NVSStore::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    auto it = s.handles.find(h);
    if (it == s.handles.end()) return ESP_ERR_NVS_INVALID_HANDLE;
    auto& ns_data = s.data[it->second.first];
    auto kit = ns_data.find(key);
    if (kit == ns_data.end()) return ESP_ERR_NVS_NOT_FOUND;
    size_t needed = kit->second.size() + 1;
    if (!out) { *len = needed; return ESP_OK; }
    if (*len < needed) return ESP_FAIL;
    std::memcpy(out, kit->second.c_str(), needed);
    *len = needed;
    return ESP_OK;
}

inline esp_err_t nvs_set_blob(nvs_handle_t h, const char* key, const void* data, size_t len) {
    auto& s = esp32emu::nvs::NVSStore::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    auto it = s.handles.find(h);
    if (it == s.handles.end()) return ESP_ERR_NVS_INVALID_HANDLE;
    if (it->second.second != NVS_READWRITE) return ESP_FAIL;
    s.data[it->second.first][key] = std::string(reinterpret_cast<const char*>(data), len);
    return ESP_OK;
}

inline esp_err_t nvs_get_blob(nvs_handle_t h, const char* key, void* out, size_t* len) {
    auto& s = esp32emu::nvs::NVSStore::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    auto it = s.handles.find(h);
    if (it == s.handles.end()) return ESP_ERR_NVS_INVALID_HANDLE;
    auto& ns_data = s.data[it->second.first];
    auto kit = ns_data.find(key);
    if (kit == ns_data.end()) return ESP_ERR_NVS_NOT_FOUND;
    if (!out) { *len = kit->second.size(); return ESP_OK; }
    if (*len < kit->second.size()) return ESP_FAIL;
    std::memcpy(out, kit->second.data(), kit->second.size());
    *len = kit->second.size();
    return ESP_OK;
}

inline esp_err_t nvs_erase_key(nvs_handle_t h, const char* key) {
    auto& s = esp32emu::nvs::NVSStore::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    auto it = s.handles.find(h);
    if (it == s.handles.end()) return ESP_ERR_NVS_INVALID_HANDLE;
    s.data[it->second.first].erase(key);
    return ESP_OK;
}

inline esp_err_t nvs_erase_all(nvs_handle_t h) {
    auto& s = esp32emu::nvs::NVSStore::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    auto it = s.handles.find(h);
    if (it == s.handles.end()) return ESP_ERR_NVS_INVALID_HANDLE;
    s.data[it->second.first].clear();
    return ESP_OK;
}

inline esp_err_t nvs_commit(nvs_handle_t) {
    return ESP_OK; // no-op in emulator
}

// ── Additional typed getters/setters (i8, u8, i16, u16, i64, u64) ──
#define NVS_TYPED_SET(suffix, type) \
inline esp_err_t nvs_set_##suffix(nvs_handle_t h, const char* key, type value) { \
    auto& s = esp32emu::nvs::NVSStore::instance(); \
    std::lock_guard<std::mutex> lk(s.mtx); \
    auto it = s.handles.find(h); \
    if (it == s.handles.end()) return ESP_ERR_NVS_INVALID_HANDLE; \
    if (it->second.second != NVS_READWRITE) return ESP_FAIL; \
    std::string blob(reinterpret_cast<const char*>(&value), sizeof(value)); \
    s.data[it->second.first][key] = blob; \
    return ESP_OK; \
}

#define NVS_TYPED_GET(suffix, type) \
inline esp_err_t nvs_get_##suffix(nvs_handle_t h, const char* key, type* out) { \
    auto& s = esp32emu::nvs::NVSStore::instance(); \
    std::lock_guard<std::mutex> lk(s.mtx); \
    auto it = s.handles.find(h); \
    if (it == s.handles.end()) return ESP_ERR_NVS_INVALID_HANDLE; \
    auto& ns_data = s.data[it->second.first]; \
    auto kit = ns_data.find(key); \
    if (kit == ns_data.end()) return ESP_ERR_NVS_NOT_FOUND; \
    if (kit->second.size() != sizeof(type)) return ESP_FAIL; \
    std::memcpy(out, kit->second.data(), sizeof(type)); \
    return ESP_OK; \
}

NVS_TYPED_SET(i8, int8_t)
NVS_TYPED_GET(i8, int8_t)
NVS_TYPED_SET(u8, uint8_t)
NVS_TYPED_GET(u8, uint8_t)
NVS_TYPED_SET(i16, int16_t)
NVS_TYPED_GET(i16, int16_t)
NVS_TYPED_SET(u16, uint16_t)
NVS_TYPED_GET(u16, uint16_t)
NVS_TYPED_SET(i64, int64_t)
NVS_TYPED_GET(i64, int64_t)
NVS_TYPED_SET(u64, uint64_t)
NVS_TYPED_GET(u64, uint64_t)

#undef NVS_TYPED_SET
#undef NVS_TYPED_GET
