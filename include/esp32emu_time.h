#pragma once
#include <chrono>
#include <thread>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <string>

namespace esp32emu {
inline auto& boot_time() {
    static auto t = std::chrono::steady_clock::now();
    return t;
}
}

inline unsigned long millis() {
    auto now = std::chrono::steady_clock::now();
    return (unsigned long)std::chrono::duration_cast<std::chrono::milliseconds>(
        now - esp32emu::boot_time()).count();
}

inline unsigned long micros() {
    auto now = std::chrono::steady_clock::now();
    return (unsigned long)std::chrono::duration_cast<std::chrono::microseconds>(
        now - esp32emu::boot_time()).count();
}

inline void delay(unsigned long ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

inline void delayMicroseconds(unsigned int us) {
    std::this_thread::sleep_for(std::chrono::microseconds(us));
}

// ── Arduino ESP32 configTime / getLocalTime ─────────────────────────
namespace esp32emu {
struct TimeConfig {
    long gmt_offset_sec = 0;
    int daylight_offset_sec = 0;
    std::string ntp_server1;
    std::string ntp_server2;
    std::string ntp_server3;
    bool configured = false;

    static TimeConfig& instance() {
        static TimeConfig tc;
        return tc;
    }
    void reset() { *this = TimeConfig{}; }
};
} // namespace esp32emu

inline void configTime(long gmtOffset_sec, int daylightOffset_sec,
                       const char* server1, const char* server2 = nullptr,
                       const char* server3 = nullptr) {
    auto& tc = esp32emu::TimeConfig::instance();
    tc.gmt_offset_sec = gmtOffset_sec;
    tc.daylight_offset_sec = daylightOffset_sec;
    tc.ntp_server1 = server1 ? server1 : "";
    tc.ntp_server2 = server2 ? server2 : "";
    tc.ntp_server3 = server3 ? server3 : "";
    tc.configured = true;
    fprintf(stderr, "[esp32emu] configTime: GMT%+ld DST%+d server=%s\n",
            gmtOffset_sec, daylightOffset_sec, server1 ? server1 : "");
}

inline bool getLocalTime(struct tm* info, uint32_t ms = 5000) {
    (void)ms;
    auto& tc = esp32emu::TimeConfig::instance();
    time_t now = time(nullptr);
    // Apply configured offset (simulate timezone without actually calling tzset)
    now += tc.gmt_offset_sec + tc.daylight_offset_sec;
    struct tm* result = gmtime(&now);
    if (!result) return false;
    *info = *result;
    return true;
}

inline void configTzTime(const char* tz, const char* server1,
                         const char* server2 = nullptr,
                         const char* server3 = nullptr) {
    (void)tz;
    configTime(0, 0, server1, server2, server3);
    fprintf(stderr, "[esp32emu] configTzTime: tz=%s\n", tz ? tz : "");
}
