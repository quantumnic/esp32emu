#pragma once
// esp32emu — ADC driver mock
// Provides adc1_config_width, adc1_config_channel_atten, adc1_get_raw, adc2_get_raw.

#include <cstdint>
#include <map>
#include <mutex>
#include <cstdlib>

typedef enum { ADC_WIDTH_BIT_9 = 0, ADC_WIDTH_BIT_10, ADC_WIDTH_BIT_11, ADC_WIDTH_BIT_12 } adc_bits_width_t;
typedef enum { ADC_ATTEN_DB_0 = 0, ADC_ATTEN_DB_2_5, ADC_ATTEN_DB_6, ADC_ATTEN_DB_11 } adc_atten_t;
typedef enum { ADC1_CHANNEL_0 = 0, ADC1_CHANNEL_1, ADC1_CHANNEL_2, ADC1_CHANNEL_3,
               ADC1_CHANNEL_4, ADC1_CHANNEL_5, ADC1_CHANNEL_6, ADC1_CHANNEL_7 } adc1_channel_t;
typedef enum { ADC2_CHANNEL_0 = 0, ADC2_CHANNEL_1, ADC2_CHANNEL_2, ADC2_CHANNEL_3,
               ADC2_CHANNEL_4, ADC2_CHANNEL_5, ADC2_CHANNEL_6, ADC2_CHANNEL_7,
               ADC2_CHANNEL_8, ADC2_CHANNEL_9 } adc2_channel_t;

#ifndef ESP_OK
#define ESP_OK 0
#endif

namespace esp32emu {
namespace adc {

struct ADCState {
    std::mutex mtx;
    adc_bits_width_t width = ADC_WIDTH_BIT_12;
    std::map<int, int> injected_adc1; // channel -> value
    std::map<int, int> injected_adc2;

    static ADCState& instance() {
        static ADCState s;
        return s;
    }

    int maxValue() const {
        switch (width) {
            case ADC_WIDTH_BIT_9:  return 511;
            case ADC_WIDTH_BIT_10: return 1023;
            case ADC_WIDTH_BIT_11: return 2047;
            case ADC_WIDTH_BIT_12: return 4095;
        }
        return 4095;
    }
};

} // namespace adc
} // namespace esp32emu

inline int adc1_config_width(adc_bits_width_t w) {
    auto& s = esp32emu::adc::ADCState::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    s.width = w;
    return ESP_OK;
}

inline int adc1_config_channel_atten(adc1_channel_t, adc_atten_t) {
    return ESP_OK; // no-op
}

inline int adc1_get_raw(adc1_channel_t ch) {
    auto& s = esp32emu::adc::ADCState::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    auto it = s.injected_adc1.find(static_cast<int>(ch));
    if (it != s.injected_adc1.end()) return it->second;
    return std::rand() % (s.maxValue() + 1);
}

inline int adc2_config_channel_atten(adc2_channel_t, adc_atten_t) {
    return ESP_OK;
}

inline int adc2_get_raw(adc2_channel_t ch, adc_bits_width_t, int* raw) {
    auto& s = esp32emu::adc::ADCState::instance();
    std::lock_guard<std::mutex> lk(s.mtx);
    auto it = s.injected_adc2.find(static_cast<int>(ch));
    if (it != s.injected_adc2.end()) { *raw = it->second; return ESP_OK; }
    *raw = std::rand() % (s.maxValue() + 1);
    return ESP_OK;
}

// Test helpers: inject specific values
namespace esp32emu_adc_test {
    inline void injectADC1(adc1_channel_t ch, int val) {
        auto& s = esp32emu::adc::ADCState::instance();
        std::lock_guard<std::mutex> lk(s.mtx);
        s.injected_adc1[static_cast<int>(ch)] = val;
    }
    inline void injectADC2(adc2_channel_t ch, int val) {
        auto& s = esp32emu::adc::ADCState::instance();
        std::lock_guard<std::mutex> lk(s.mtx);
        s.injected_adc2[static_cast<int>(ch)] = val;
    }
    inline void clearInjections() {
        auto& s = esp32emu::adc::ADCState::instance();
        std::lock_guard<std::mutex> lk(s.mtx);
        s.injected_adc1.clear();
        s.injected_adc2.clear();
    }
}
