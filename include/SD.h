#pragma once
// esp32emu — SD card library mock
// Simulates SD card operations using host filesystem in a temp directory

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <map>

#define FILE_READ   "r"
#define FILE_WRITE  "w"
#define FILE_APPEND "a"

namespace esp32emu {

class SDFile {
public:
    SDFile() = default;
    SDFile(const std::string& path, const std::string& mode) : path_(path) {
        if (mode == "r") {
            ifs_.open(path, std::ios::binary);
            readable_ = true;
            valid_ = ifs_.is_open();
        } else {
            auto flags = std::ios::binary;
            if (mode == "a") flags |= std::ios::app;
            else flags |= std::ios::trunc;
            ofs_.open(path, flags);
            writable_ = true;
            valid_ = ofs_.is_open();
        }
    }

    explicit operator bool() const { return valid_; }

    int available() {
        if (!readable_ || !ifs_.is_open()) return 0;
        auto cur = ifs_.tellg();
        ifs_.seekg(0, std::ios::end);
        auto end = ifs_.tellg();
        ifs_.seekg(cur);
        return (int)(end - cur);
    }

    int read() {
        if (!readable_) return -1;
        return ifs_.get();
    }

    int read(uint8_t* buf, size_t len) {
        if (!readable_) return 0;
        ifs_.read(reinterpret_cast<char*>(buf), len);
        return (int)ifs_.gcount();
    }

    size_t write(uint8_t val) {
        if (!writable_) return 0;
        ofs_.put(val);
        return 1;
    }

    size_t write(const uint8_t* buf, size_t len) {
        if (!writable_) return 0;
        ofs_.write(reinterpret_cast<const char*>(buf), len);
        return len;
    }

    size_t print(const char* s) {
        return write(reinterpret_cast<const uint8_t*>(s), strlen(s));
    }

    size_t println(const char* s) {
        size_t n = print(s);
        n += print("\n");
        return n;
    }

    size_t println(int val) {
        auto s = std::to_string(val);
        return println(s.c_str());
    }

    void close() {
        ifs_.close();
        ofs_.close();
        valid_ = false;
    }

    const char* name() const { return path_.c_str(); }

    size_t size() {
        if (std::filesystem::exists(path_))
            return (size_t)std::filesystem::file_size(path_);
        return 0;
    }

    bool isDirectory() {
        return std::filesystem::is_directory(path_);
    }

    void flush() { if (writable_) ofs_.flush(); }

    // String read helper
    std::string readString() {
        std::ostringstream ss;
        ss << ifs_.rdbuf();
        return ss.str();
    }

private:
    std::string path_;
    std::ifstream ifs_;
    std::ofstream ofs_;
    bool valid_ = false;
    bool readable_ = false;
    bool writable_ = false;
};

class SDClass {
public:
    static SDClass& instance() {
        static SDClass inst;
        return inst;
    }

    bool begin(uint8_t cs_pin = 5) {
        (void)cs_pin;
        root_ = std::filesystem::temp_directory_path() / "esp32emu_sd";
        std::filesystem::create_directories(root_);
        mounted_ = true;
        fprintf(stderr, "[esp32emu] SD card mounted at %s (CS pin %d)\n",
                root_.c_str(), cs_pin);
        return true;
    }

    void end() {
        mounted_ = false;
        fprintf(stderr, "[esp32emu] SD card unmounted\n");
    }

    SDFile open(const char* path, const char* mode = FILE_READ) {
        if (!mounted_) return SDFile();
        auto full = root_ / (path[0] == '/' ? path + 1 : path);
        if (std::string(mode) != "r") {
            std::filesystem::create_directories(full.parent_path());
        }
        return SDFile(full.string(), mode);
    }

    bool exists(const char* path) {
        if (!mounted_) return false;
        auto full = root_ / (path[0] == '/' ? path + 1 : path);
        return std::filesystem::exists(full);
    }

    bool mkdir(const char* path) {
        if (!mounted_) return false;
        auto full = root_ / (path[0] == '/' ? path + 1 : path);
        return std::filesystem::create_directories(full);
    }

    bool remove(const char* path) {
        if (!mounted_) return false;
        auto full = root_ / (path[0] == '/' ? path + 1 : path);
        return std::filesystem::remove(full);
    }

    bool rmdir(const char* path) {
        if (!mounted_) return false;
        auto full = root_ / (path[0] == '/' ? path + 1 : path);
        return std::filesystem::remove(full);
    }

    uint64_t totalBytes() { return 4ULL * 1024 * 1024 * 1024; } // 4GB
    uint64_t usedBytes() { return 1024 * 1024; } // 1MB used

private:
    std::filesystem::path root_;
    bool mounted_ = false;
};

} // namespace esp32emu

// Arduino-compatible global
inline esp32emu::SDClass& SD = esp32emu::SDClass::instance();

// Use the esp32emu File type when SD.h is included
using File = esp32emu::SDFile;
