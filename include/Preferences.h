#pragma once
// esp32emu — Preferences (NVS) mock using in-memory storage
#include <cstdint>
#include <string>
#include <map>
#include <cstring>
#include <cstdio>

class Preferences {
public:
    bool begin(const char* name, bool readOnly = false) {
        ns_ = name ? name : "";
        readOnly_ = readOnly;
        return true;
    }

    void end() { ns_.clear(); }

    bool clear() {
        if (readOnly_) return false;
        auto it = store().begin();
        while (it != store().end()) {
            if (it->first.rfind(ns_ + "/", 0) == 0)
                it = store().erase(it);
            else
                ++it;
        }
        return true;
    }

    bool remove(const char* key) {
        if (readOnly_) return false;
        return store().erase(fqn(key)) > 0;
    }

    // --- Put ---
    size_t putChar(const char* key, int8_t val)    { return putT(key, val); }
    size_t putUChar(const char* key, uint8_t val)  { return putT(key, val); }
    size_t putShort(const char* key, int16_t val)  { return putT(key, val); }
    size_t putUShort(const char* key, uint16_t val){ return putT(key, val); }
    size_t putInt(const char* key, int32_t val)    { return putT(key, val); }
    size_t putUInt(const char* key, uint32_t val)  { return putT(key, val); }
    size_t putLong(const char* key, int32_t val)   { return putT(key, val); }
    size_t putULong(const char* key, uint32_t val) { return putT(key, val); }
    size_t putLong64(const char* key, int64_t val) { return putT(key, val); }
    size_t putULong64(const char* key, uint64_t val){ return putT(key, val); }
    size_t putFloat(const char* key, float val)    { return putT(key, val); }
    size_t putDouble(const char* key, double val)  { return putT(key, val); }
    size_t putBool(const char* key, bool val)      { return putT(key, val); }

    size_t putString(const char* key, const char* val) {
        if (readOnly_) return 0;
        store()[fqn(key)] = std::string(val ? val : "");
        return strlen(val ? val : "");
    }
    size_t putString(const char* key, const std::string& val) {
        return putString(key, val.c_str());
    }

    size_t putBytes(const char* key, const void* buf, size_t len) {
        if (readOnly_) return 0;
        store()[fqn(key)] = std::string(reinterpret_cast<const char*>(buf), len);
        return len;
    }

    // --- Get ---
    int8_t   getChar(const char* key, int8_t def = 0)      { return getT(key, def); }
    uint8_t  getUChar(const char* key, uint8_t def = 0)     { return getT(key, def); }
    int16_t  getShort(const char* key, int16_t def = 0)     { return getT(key, def); }
    uint16_t getUShort(const char* key, uint16_t def = 0)   { return getT(key, def); }
    int32_t  getInt(const char* key, int32_t def = 0)       { return getT(key, def); }
    uint32_t getUInt(const char* key, uint32_t def = 0)     { return getT(key, def); }
    int32_t  getLong(const char* key, int32_t def = 0)      { return getT(key, def); }
    uint32_t getULong(const char* key, uint32_t def = 0)    { return getT(key, def); }
    int64_t  getLong64(const char* key, int64_t def = 0)    { return getT(key, def); }
    uint64_t getULong64(const char* key, uint64_t def = 0)  { return getT(key, def); }
    float    getFloat(const char* key, float def = 0)       { return getT(key, def); }
    double   getDouble(const char* key, double def = 0)     { return getT(key, def); }
    bool     getBool(const char* key, bool def = false)     { return getT(key, def); }

    std::string getString(const char* key, const char* def = "") {
        auto it = store().find(fqn(key));
        if (it == store().end()) return def ? def : "";
        return it->second;
    }

    size_t getBytes(const char* key, void* buf, size_t maxLen) {
        auto it = store().find(fqn(key));
        if (it == store().end()) return 0;
        size_t n = std::min(maxLen, it->second.size());
        memcpy(buf, it->second.data(), n);
        return n;
    }

    bool isKey(const char* key) {
        return store().find(fqn(key)) != store().end();
    }

    size_t freeEntries() { return 999; }

private:
    std::string ns_;
    bool readOnly_ = false;

    std::string fqn(const char* key) { return ns_ + "/" + key; }

    // Global store shared across instances
    static std::map<std::string, std::string>& store() {
        static std::map<std::string, std::string> s;
        return s;
    }

    template<typename T>
    size_t putT(const char* key, T val) {
        if (readOnly_) return 0;
        store()[fqn(key)] = std::string(reinterpret_cast<const char*>(&val), sizeof(T));
        return sizeof(T);
    }

    template<typename T>
    T getT(const char* key, T def) {
        auto it = store().find(fqn(key));
        if (it == store().end() || it->second.size() != sizeof(T)) return def;
        T val;
        memcpy(&val, it->second.data(), sizeof(T));
        return val;
    }
};
