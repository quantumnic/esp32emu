#pragma once
#include <cstdint>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// ADC oneshot driver - ESP-IDF 5.x new API
typedef void* adc_oneshot_unit_handle_t;

typedef enum {
    ADC_UNIT_1 = 0,
    ADC_UNIT_2 = 1,
} adc_unit_t_oneshot;

typedef enum {
    ADC_CHANNEL_0 = 0, ADC_CHANNEL_1, ADC_CHANNEL_2, ADC_CHANNEL_3,
    ADC_CHANNEL_4, ADC_CHANNEL_5, ADC_CHANNEL_6, ADC_CHANNEL_7,
    ADC_CHANNEL_8, ADC_CHANNEL_9,
    ADC_CHANNEL_MAX
} adc_channel_t_oneshot;

typedef enum {
    ADC_ATTEN_DB_0 = 0,
    ADC_ATTEN_DB_2_5 = 1,
    ADC_ATTEN_DB_6 = 2,
    ADC_ATTEN_DB_12 = 3,
} adc_atten_t_oneshot;

typedef enum {
    ADC_BITWIDTH_DEFAULT = 0,
    ADC_BITWIDTH_9 = 9,
    ADC_BITWIDTH_10 = 10,
    ADC_BITWIDTH_11 = 11,
    ADC_BITWIDTH_12 = 12,
    ADC_BITWIDTH_13 = 13,
} adc_bitwidth_t_oneshot;

typedef struct {
    int unit_id;
    int clk_src;
    int ulp_mode;
} adc_oneshot_unit_init_cfg_t;

typedef struct {
    int atten;
    int bitwidth;
} adc_oneshot_chan_cfg_t;

// Mock state
#ifndef ESP_ADC_ONESHOT_IMPL
#define ESP_ADC_ONESHOT_IMPL

static int _adc_oneshot_values[2][10] = {
    {2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048},
    {2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048, 2048},
};
static int _adc_oneshot_unit = 0;
static bool _adc_oneshot_configured[2][10] = {};

inline esp_err_t adc_oneshot_new_unit(const adc_oneshot_unit_init_cfg_t* cfg, adc_oneshot_unit_handle_t* handle) {
    if (!cfg || !handle) return ESP_ERR_INVALID_ARG;
    _adc_oneshot_unit = cfg->unit_id;
    // Store unit in handle as integer cast
    *handle = (void*)(intptr_t)(cfg->unit_id + 1);
    return ESP_OK;
}

inline esp_err_t adc_oneshot_config_channel(adc_oneshot_unit_handle_t handle, int channel, const adc_oneshot_chan_cfg_t* cfg) {
    if (!handle || !cfg || channel < 0 || channel >= 10) return ESP_ERR_INVALID_ARG;
    int unit = (int)(intptr_t)handle - 1;
    if (unit < 0 || unit > 1) return ESP_ERR_INVALID_ARG;
    _adc_oneshot_configured[unit][channel] = true;
    return ESP_OK;
}

inline esp_err_t adc_oneshot_read(adc_oneshot_unit_handle_t handle, int channel, int* out_raw) {
    if (!handle || !out_raw || channel < 0 || channel >= 10) return ESP_ERR_INVALID_ARG;
    int unit = (int)(intptr_t)handle - 1;
    if (unit < 0 || unit > 1) return ESP_ERR_INVALID_ARG;
    *out_raw = _adc_oneshot_values[unit][channel];
    return ESP_OK;
}

inline esp_err_t adc_oneshot_del_unit(adc_oneshot_unit_handle_t handle) {
    (void)handle;
    return ESP_OK;
}

// Test helpers
inline void _test_adc_oneshot_set_value(int unit, int channel, int value) {
    if (unit >= 0 && unit < 2 && channel >= 0 && channel < 10)
        _adc_oneshot_values[unit][channel] = value;
}

inline void _test_adc_oneshot_reset() {
    for (int u = 0; u < 2; u++)
        for (int c = 0; c < 10; c++) {
            _adc_oneshot_values[u][c] = 2048;
            _adc_oneshot_configured[u][c] = false;
        }
}

#endif

#ifdef __cplusplus
}
#endif
