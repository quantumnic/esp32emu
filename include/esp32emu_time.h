#pragma once
#include <chrono>
#include <thread>
#include <cstdint>

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
