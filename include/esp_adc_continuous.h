#pragma once
#include <cstdint>
#include <cstring>
#include <vector>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// ADC continuous (DMA) mode mock for ESP-IDF

typedef void* adc_continuous_handle_t;

typedef enum {
    ADC_CONV_SINGLE_UNIT_1 = 1,
    ADC_CONV_SINGLE_UNIT_2 = 2,
    ADC_CONV_BOTH_UNIT = 3,
    ADC_CONV_ALTER_UNIT = 7
} adc_digi_convert_mode_t;

typedef enum {
    ADC_CONV_ELAPSE_FORMAT = 0,
    ADC_CONV_MULTI_FORMAT = 1
} adc_digi_output_format_t;

typedef struct {
    uint8_t atten;
    uint8_t channel;
    uint8_t unit;
    uint8_t bit_width;
} adc_digi_pattern_config_t;

typedef struct {
    uint32_t pattern_num;
    adc_digi_pattern_config_t *adc_pattern;
    uint32_t sample_freq_hz;
    adc_digi_convert_mode_t conv_mode;
    adc_digi_output_format_t format;
} adc_continuous_config_t;

typedef struct {
    uint32_t max_store_buf_size;
    uint32_t conv_frame_size;
} adc_continuous_handle_cfg_t;

typedef struct {
    uint16_t val;
    uint16_t channel : 4;
    uint16_t unit : 1;
} adc_digi_output_data_t;

typedef struct {
    void (*on_conv_done)(adc_continuous_handle_t, void*);
    void (*on_pool_ovf)(adc_continuous_handle_t, void*);
} adc_continuous_evt_cbs_t;

// Internal state for mock
namespace esp32emu_adc_cont {
    struct State {
        bool initialized = false;
        bool started = false;
        uint32_t sample_freq = 0;
        uint32_t conv_frame_size = 0;
        uint32_t max_store_buf_size = 0;
        std::vector<adc_digi_pattern_config_t> patterns;
        std::vector<uint8_t> injected_data;
    };
    inline State& get() { static State s; return s; }
    inline void reset() { get() = State{}; }
}

inline esp_err_t adc_continuous_new_handle(const adc_continuous_handle_cfg_t *cfg,
                                            adc_continuous_handle_t *handle) {
    auto& s = esp32emu_adc_cont::get();
    s.initialized = true;
    s.conv_frame_size = cfg->conv_frame_size;
    s.max_store_buf_size = cfg->max_store_buf_size;
    *handle = (void*)1;
    return ESP_OK;
}

inline esp_err_t adc_continuous_config(adc_continuous_handle_t handle,
                                        const adc_continuous_config_t *cfg) {
    (void)handle;
    auto& s = esp32emu_adc_cont::get();
    s.sample_freq = cfg->sample_freq_hz;
    s.patterns.clear();
    for (uint32_t i = 0; i < cfg->pattern_num; i++) {
        s.patterns.push_back(cfg->adc_pattern[i]);
    }
    return ESP_OK;
}

inline esp_err_t adc_continuous_register_event_callbacks(adc_continuous_handle_t handle,
                                                          const adc_continuous_evt_cbs_t *cbs,
                                                          void *user_data) {
    (void)handle; (void)cbs; (void)user_data;
    return ESP_OK;
}

inline esp_err_t adc_continuous_start(adc_continuous_handle_t handle) {
    (void)handle;
    esp32emu_adc_cont::get().started = true;
    return ESP_OK;
}

inline esp_err_t adc_continuous_stop(adc_continuous_handle_t handle) {
    (void)handle;
    esp32emu_adc_cont::get().started = false;
    return ESP_OK;
}

inline esp_err_t adc_continuous_read(adc_continuous_handle_t handle,
                                      uint8_t *buf, uint32_t length_max,
                                      uint32_t *out_length, uint32_t timeout_ms) {
    (void)handle; (void)timeout_ms;
    auto& s = esp32emu_adc_cont::get();
    if (!s.started) return ESP_ERR_INVALID_STATE;
    if (s.injected_data.empty()) {
        // Generate default data from patterns
        uint32_t to_fill = length_max;
        uint32_t written = 0;
        while (written + sizeof(adc_digi_output_data_t) <= to_fill && !s.patterns.empty()) {
            for (auto& p : s.patterns) {
                if (written + sizeof(adc_digi_output_data_t) > to_fill) break;
                adc_digi_output_data_t d;
                d.val = 2048; // mid-scale
                d.channel = p.channel;
                d.unit = p.unit;
                memcpy(buf + written, &d, sizeof(d));
                written += sizeof(d);
            }
        }
        *out_length = written;
    } else {
        uint32_t to_copy = length_max < (uint32_t)s.injected_data.size()
                           ? length_max : (uint32_t)s.injected_data.size();
        memcpy(buf, s.injected_data.data(), to_copy);
        s.injected_data.erase(s.injected_data.begin(),
                               s.injected_data.begin() + to_copy);
        *out_length = to_copy;
    }
    return ESP_OK;
}

inline esp_err_t adc_continuous_deinit(adc_continuous_handle_t handle) {
    (void)handle;
    esp32emu_adc_cont::reset();
    return ESP_OK;
}

// Test helpers
inline void test_adc_continuous_inject(const uint8_t *data, uint32_t len) {
    auto& s = esp32emu_adc_cont::get();
    s.injected_data.insert(s.injected_data.end(), data, data + len);
}

inline bool test_adc_continuous_is_started() {
    return esp32emu_adc_cont::get().started;
}

inline uint32_t test_adc_continuous_get_sample_freq() {
    return esp32emu_adc_cont::get().sample_freq;
}

#ifdef __cplusplus
}
#endif
