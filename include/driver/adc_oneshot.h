#pragma once
// esp32emu: ESP-IDF ADC Oneshot driver mock (new API)
#include "esp_err.h"
#include <cstdint>
#include <map>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ADC_UNIT_1 = 0,
    ADC_UNIT_2 = 1,
} adc_unit_t_oneshot;

typedef enum {
    ADC_CHANNEL_0_ONESHOT = 0,
    ADC_CHANNEL_1_ONESHOT,
    ADC_CHANNEL_2_ONESHOT,
    ADC_CHANNEL_3_ONESHOT,
    ADC_CHANNEL_4_ONESHOT,
    ADC_CHANNEL_5_ONESHOT,
    ADC_CHANNEL_6_ONESHOT,
    ADC_CHANNEL_7_ONESHOT,
    ADC_CHANNEL_8_ONESHOT,
    ADC_CHANNEL_9_ONESHOT,
    ADC_CHANNEL_MAX_ONESHOT,
} adc_channel_t_oneshot;

typedef enum {
    ADC_ATTEN_DB_0_ONESHOT = 0,
    ADC_ATTEN_DB_2_5_ONESHOT,
    ADC_ATTEN_DB_6_ONESHOT,
    ADC_ATTEN_DB_12_ONESHOT,
} adc_atten_t_oneshot;

typedef enum {
    ADC_BITWIDTH_DEFAULT_ONESHOT = 0,
    ADC_BITWIDTH_9_ONESHOT = 9,
    ADC_BITWIDTH_10_ONESHOT = 10,
    ADC_BITWIDTH_11_ONESHOT = 11,
    ADC_BITWIDTH_12_ONESHOT = 12,
    ADC_BITWIDTH_13_ONESHOT = 13,
} adc_bitwidth_t_oneshot;

typedef struct {
    adc_unit_t_oneshot unit_id;
} adc_oneshot_unit_init_cfg_t;

typedef struct {
    adc_bitwidth_t_oneshot bitwidth;
    adc_atten_t_oneshot atten;
} adc_oneshot_chan_cfg_t;

typedef struct adc_oneshot_unit_ctx_t* adc_oneshot_unit_handle_t;

#ifdef __cplusplus
}
#endif

// Implementation (header-only for esp32emu)
#ifdef __cplusplus

struct adc_oneshot_unit_ctx_t {
    adc_unit_t_oneshot unit;
    bool configured[ADC_CHANNEL_MAX_ONESHOT] = {};
    int values[ADC_CHANNEL_MAX_ONESHOT] = {};
    adc_atten_t_oneshot atten[ADC_CHANNEL_MAX_ONESHOT] = {};
    adc_bitwidth_t_oneshot bitwidth[ADC_CHANNEL_MAX_ONESHOT] = {};
};

namespace esp32emu_adc_oneshot {
    inline adc_oneshot_unit_ctx_t* units[2] = {};

    // Test helper: inject a raw reading
    inline void setRawValue(int unit, int channel, int value) {
        if (units[unit] && channel < ADC_CHANNEL_MAX_ONESHOT) {
            units[unit]->values[channel] = value;
        }
    }
}

inline esp_err_t adc_oneshot_new_unit(const adc_oneshot_unit_init_cfg_t* cfg, adc_oneshot_unit_handle_t* handle) {
    auto* ctx = new adc_oneshot_unit_ctx_t();
    ctx->unit = cfg->unit_id;
    for (int i = 0; i < ADC_CHANNEL_MAX_ONESHOT; i++) ctx->values[i] = 2048; // mid-range default
    esp32emu_adc_oneshot::units[cfg->unit_id] = ctx;
    *handle = ctx;
    return ESP_OK;
}

inline esp_err_t adc_oneshot_config_channel(adc_oneshot_unit_handle_t handle,
                                              int channel,
                                              const adc_oneshot_chan_cfg_t* cfg) {
    if (!handle || channel < 0 || channel >= ADC_CHANNEL_MAX_ONESHOT) return ESP_ERR_INVALID_ARG;
    handle->configured[channel] = true;
    handle->atten[channel] = cfg->atten;
    handle->bitwidth[channel] = cfg->bitwidth;
    return ESP_OK;
}

inline esp_err_t adc_oneshot_read(adc_oneshot_unit_handle_t handle, int channel, int* out_raw) {
    if (!handle || channel < 0 || channel >= ADC_CHANNEL_MAX_ONESHOT) return ESP_ERR_INVALID_ARG;
    *out_raw = handle->values[channel];
    return ESP_OK;
}

inline esp_err_t adc_oneshot_del_unit(adc_oneshot_unit_handle_t handle) {
    if (!handle) return ESP_ERR_INVALID_ARG;
    esp32emu_adc_oneshot::units[handle->unit] = nullptr;
    delete handle;
    return ESP_OK;
}

#endif
