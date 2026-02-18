#pragma once
// esp32emu — SPIFFS/LittleFS mock using host filesystem

#include <cstdio>
#include <cstdint>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace esp32emu {

class FSFile {
public:
    FSFile() = default;
    FSFile(const std::string& path, const char* mode) : path_(path) {
        if (std::string(mode) == "r") {
            ifs_.open(path, std::ios::binary);
            readable_ = true;
            valid_ = ifs_.is_open();
        } else {
            auto flags = std::ios::binary;
            if (std::string(mode) == "a") flags |= std::ios::app;
            else flags |= std::ios::trunc;
            ofs_.open(path, flags);
            writable_ = true;
            valid_ = ofs_.is_open();
        }
    }

    explicit operator bool() const { return valid_; }
    int available() {
        if (!readable_) return 0;
        auto c = ifs_.tellg();
        ifs_.seekg(0, std::ios::end);
        auto e = ifs_.tellg();
        ifs_.seekg(c);
        return (int)(e - c);
    }
    int read() { return readable_ ? ifs_.get() : -1; }
    size_t write(uint8_t v) { if (writable_) { ofs_.put(v); return 1; } return 0; }
    size_t write(const uint8_t* buf, size_t len) {
        if (!writable_) return 0;
        ofs_.write(reinterpret_cast<const char*>(buf), len);
        return len;
    }
    size_t print(const char* s) { return write(reinterpret_cast<const uint8_t*>(s), strlen(s)); }
    size_t println(const char* s) { size_t n = print(s); n += print("\n"); return n; }
    void close() { ifs_.close(); ofs_.close(); valid_ = false; }
    const char* name() const { return path_.c_str(); }
    size_t size() {
        if (std::filesystem::exists(path_)) return (size_t)std::filesystem::file_size(path_);
        return 0;
    }
    std::string readString() { std::ostringstream ss; ss << ifs_.rdbuf(); return ss.str(); }
    void flush() { if (writable_) ofs_.flush(); }

private:
    std::string path_;
    std::ifstream ifs_;
    std::ofstream ofs_;
    bool valid_ = false;
    bool readable_ = false;
    bool writable_ = false;
};

class FileSystem {
public:
    FileSystem(const char* name) : name_(name) {}

    bool begin(bool formatOnFail = false) {
        (void)formatOnFail;
        root_ = std::filesystem::temp_directory_path() / ("esp32emu_" + name_);
        std::filesystem::create_directories(root_);
        mounted_ = true;
        fprintf(stderr, "[esp32emu] %s mounted at %s\n", name_.c_str(), root_.c_str());
        return true;
    }

    void end() { mounted_ = false; }

    FSFile open(const char* path, const char* mode = "r") {
        if (!mounted_) return FSFile();
        auto full = root_ / (path[0] == '/' ? path + 1 : path);
        if (std::string(mode) != "r")
            std::filesystem::create_directories(full.parent_path());
        return FSFile(full.string(), mode);
    }

    bool exists(const char* path) {
        auto full = root_ / (path[0] == '/' ? path + 1 : path);
        return std::filesystem::exists(full);
    }

    bool remove(const char* path) {
        auto full = root_ / (path[0] == '/' ? path + 1 : path);
        return std::filesystem::remove(full);
    }

    bool rename(const char* from, const char* to) {
        auto f = root_ / (from[0] == '/' ? from + 1 : from);
        auto t = root_ / (to[0] == '/' ? to + 1 : to);
        std::filesystem::rename(f, t);
        return true;
    }

    bool mkdir(const char* path) {
        auto full = root_ / (path[0] == '/' ? path + 1 : path);
        return std::filesystem::create_directories(full);
    }

    size_t totalBytes() { return 1048576; }  // 1MB
    size_t usedBytes() { return 4096; }

private:
    std::string name_;
    std::filesystem::path root_;
    bool mounted_ = false;
};

} // namespace esp32emu

inline esp32emu::FileSystem SPIFFS("SPIFFS");
inline esp32emu::FileSystem LittleFS("LittleFS");
