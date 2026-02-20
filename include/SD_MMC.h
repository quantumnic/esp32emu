#pragma once
// esp32emu — SD_MMC (SDMMC interface) mock
// Provides the same filesystem API as SD.h but via the faster SDMMC bus.
// Delegates to the same in-memory VFS as SD.h / SPIFFS / LittleFS.

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

namespace esp32emu {
namespace sdmmc_detail {

struct FileEntry {
    std::vector<uint8_t> data;
    bool isDir = false;
};

inline std::map<std::string, FileEntry>& storage() {
    static std::map<std::string, FileEntry> s;
    return s;
}

inline bool& mounted() {
    static bool m = false;
    return m;
}

inline std::string normPath(const char* p) {
    std::string s(p);
    if (s.empty() || s[0] != '/') s = "/" + s;
    while (s.size() > 1 && s.back() == '/') s.pop_back();
    return s;
}

} // namespace sdmmc_detail
} // namespace esp32emu

class SDMMCFile {
public:
    SDMMCFile() = default;
    SDMMCFile(const std::string& path, bool write, bool append)
        : path_(path), valid_(true), writeMode_(write) {
        auto& st = esp32emu::sdmmc_detail::storage();
        auto it = st.find(path);
        if (write) {
            if (it == st.end()) {
                st[path] = {};
            }
            if (!append) {
                st[path].data.clear();
            }
            pos_ = append ? st[path].data.size() : 0;
        } else {
            if (it == st.end()) { valid_ = false; return; }
            if (it->second.isDir) { isDir_ = true; }
            pos_ = 0;
        }
    }

    explicit operator bool() const { return valid_; }
    bool isDirectory() const { return isDir_; }
    const char* name() const { return path_.c_str(); }
    const char* path() const { return path_.c_str(); }

    size_t size() {
        auto& st = esp32emu::sdmmc_detail::storage();
        auto it = st.find(path_);
        return it != st.end() ? it->second.data.size() : 0;
    }

    int available() {
        return (int)size() - (int)pos_;
    }

    int read() {
        auto& st = esp32emu::sdmmc_detail::storage();
        auto it = st.find(path_);
        if (it == st.end() || pos_ >= it->second.data.size()) return -1;
        return it->second.data[pos_++];
    }

    size_t read(uint8_t* buf, size_t len) {
        auto& st = esp32emu::sdmmc_detail::storage();
        auto it = st.find(path_);
        if (it == st.end()) return 0;
        size_t avail = it->second.data.size() - pos_;
        size_t n = std::min(len, avail);
        memcpy(buf, it->second.data.data() + pos_, n);
        pos_ += n;
        return n;
    }

    size_t write(const uint8_t* buf, size_t len) {
        if (!writeMode_) return 0;
        auto& d = esp32emu::sdmmc_detail::storage()[path_].data;
        if (pos_ < d.size()) {
            size_t overwrite = std::min(len, d.size() - pos_);
            memcpy(d.data() + pos_, buf, overwrite);
            if (len > overwrite) d.insert(d.end(), buf + overwrite, buf + len);
        } else {
            d.insert(d.end(), buf, buf + len);
        }
        pos_ += len;
        return len;
    }

    size_t write(uint8_t b) { return write(&b, 1); }

    size_t print(const char* s) { return write((const uint8_t*)s, strlen(s)); }
    size_t println(const char* s) { size_t n = print(s); n += print("\n"); return n; }
    size_t println() { return print("\n"); }

    bool seek(size_t pos) { pos_ = pos; return true; }
    size_t position() const { return pos_; }

    void close() { valid_ = false; }
    void flush() {}

    // Directory iteration
    SDMMCFile openNextFile() {
        if (!isDir_) return SDMMCFile();
        auto& st = esp32emu::sdmmc_detail::storage();
        std::string prefix = (path_ == "/") ? "/" : path_ + "/";
        int idx = 0;
        for (auto& [k, v] : st) {
            if (k.rfind(prefix, 0) == 0 && k != path_) {
                // Only direct children
                auto rest = k.substr(prefix.size());
                if (rest.find('/') != std::string::npos) continue;
                if (idx == dirIdx_) {
                    dirIdx_++;
                    return SDMMCFile(k, false, false);
                }
                idx++;
            }
        }
        return SDMMCFile();
    }

private:
    std::string path_;
    bool valid_ = false;
    bool writeMode_ = false;
    bool isDir_ = false;
    size_t pos_ = 0;
    int dirIdx_ = 0;
};

class SD_MMC_Class {
public:
    bool begin(const char* mountpoint = "/sdcard", bool mode1bit = false, bool format_if_mount_failed = false) {
        (void)mountpoint; (void)mode1bit; (void)format_if_mount_failed;
        esp32emu::sdmmc_detail::mounted() = true;
        fprintf(stderr, "[esp32emu] SD_MMC: mounted (%s)\n", mode1bit ? "1-bit" : "4-bit");
        return true;
    }

    void end() {
        esp32emu::sdmmc_detail::mounted() = false;
        esp32emu::sdmmc_detail::storage().clear();
    }

    SDMMCFile open(const char* path, const char* mode = "r") {
        std::string p = esp32emu::sdmmc_detail::normPath(path);
        bool write = (strchr(mode, 'w') != nullptr);
        bool append = (strchr(mode, 'a') != nullptr);
        return SDMMCFile(p, write || append, append);
    }

    bool exists(const char* path) {
        std::string p = esp32emu::sdmmc_detail::normPath(path);
        return esp32emu::sdmmc_detail::storage().count(p) > 0;
    }

    bool mkdir(const char* path) {
        std::string p = esp32emu::sdmmc_detail::normPath(path);
        auto& e = esp32emu::sdmmc_detail::storage()[p];
        e.isDir = true;
        return true;
    }

    bool remove(const char* path) {
        std::string p = esp32emu::sdmmc_detail::normPath(path);
        return esp32emu::sdmmc_detail::storage().erase(p) > 0;
    }

    bool rmdir(const char* path) { return remove(path); }

    bool rename(const char* from, const char* to) {
        std::string pf = esp32emu::sdmmc_detail::normPath(from);
        std::string pt = esp32emu::sdmmc_detail::normPath(to);
        auto& st = esp32emu::sdmmc_detail::storage();
        auto it = st.find(pf);
        if (it == st.end()) return false;
        st[pt] = std::move(it->second);
        st.erase(it);
        return true;
    }

    uint64_t totalBytes() { return 16ULL * 1024 * 1024 * 1024; }  // 16 GB
    uint64_t usedBytes() {
        uint64_t total = 0;
        for (auto& [k, v] : esp32emu::sdmmc_detail::storage())
            total += v.data.size();
        return total;
    }

    uint8_t cardType() { return 2; } // CARD_SDHC
};

extern SD_MMC_Class SD_MMC;

#ifndef ESP32EMU_SD_MMC_IMPL
inline SD_MMC_Class SD_MMC;
#endif
