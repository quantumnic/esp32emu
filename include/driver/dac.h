#pragma once
// esp32emu — DAC driver mock
// Provides dac_output_enable, dac_output_voltage, dac_output_disable.

#include <cstdint>
#include <cstdio>
#include <mutex>
#include <map>

typedef enum { DAC_CHANNEL_1 = 0, DAC_CHANNEL_2 } dac_channel_t;

#ifndef ESP_OK
#define ESP_OK 0
#endif
#ifndef ESP_FAIL
#define ESP_FAIL -1
#endif

namespace esp32emu {
namespace dac {

struct DACState {
    std::mutex mtx;
    std::map<int, bool> enabled;
    std::map<int, uint8_t> voltage; // 0-255

    static DACState& instance() {
        static DACState s;
        return s;
    }
};

} // namespace dac
} // namespace esp32emu

inline int dac_output_enable(dac_channel_t ch) {
    auto& s = esp32emu::dac::DACState::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    s.enabled[static_cast<int>(ch)] = true;
    s.voltage[static_cast<int>(ch)] = 0;
    printf("[esp32emu] DAC channel %d enabled\n", static_cast<int>(ch));
    return ESP_OK;
}

inline int dac_output_voltage(dac_channel_t ch, uint8_t val) {
    auto& s = esp32emu::dac::DACState::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    if (!s.enabled[static_cast<int>(ch)]) return ESP_FAIL;
    s.voltage[static_cast<int>(ch)] = val;
    return ESP_OK;
}

inline int dac_output_disable(dac_channel_t ch) {
    auto& s = esp32emu::dac::DACState::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    s.enabled[static_cast<int>(ch)] = false;
    printf("[esp32emu] DAC channel %d disabled\n", static_cast<int>(ch));
    return ESP_OK;
}

// Test helpers
namespace esp32emu_dac_test {
    inline uint8_t getVoltage(dac_channel_t ch) {
        auto& s = esp32emu::dac::DACState::instance();
        std::lock_guard<std::mutex> lk(s.mtx);
        return s.voltage[static_cast<int>(ch)];
    }
    inline bool isEnabled(dac_channel_t ch) {
        auto& s = esp32emu::dac::DACState::instance();
        std::lock_guard<std::mutex> lk(s.mtx);
        return s.enabled[static_cast<int>(ch)];
    }
}
