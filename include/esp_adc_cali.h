#pragma once
// esp32emu — ESP-IDF v5 ADC calibration mock

#include "esp_err.h"
#include <cstdint>
#include <cstdio>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct adc_cali_scheme_t* adc_cali_handle_t;

typedef enum {
    ADC_ATTEN_DB_0 = 0,
    ADC_ATTEN_DB_2_5 = 1,
    ADC_ATTEN_DB_6 = 2,
    ADC_ATTEN_DB_12 = 3,   // ESP-IDF v5 name
    ADC_ATTEN_DB_11 = 3,   // Alias (older name)
} adc_atten_t_cali;

typedef enum {
    ADC_BITWIDTH_9 = 9,
    ADC_BITWIDTH_10 = 10,
    ADC_BITWIDTH_11 = 11,
    ADC_BITWIDTH_12 = 12,
    ADC_BITWIDTH_13 = 13,
    ADC_BITWIDTH_DEFAULT = 12,
} adc_bitwidth_t_cali;

typedef enum {
    ADC_UNIT_1 = 0,
    ADC_UNIT_2 = 1,
} adc_unit_t_cali;

typedef struct {
    adc_unit_t_cali unit_id;
    int atten;              // adc_atten_t
    int bitwidth;           // adc_bitwidth_t
} adc_cali_line_fitting_config_t;

typedef struct {
    adc_unit_t_cali unit_id;
    int atten;
    int bitwidth;
} adc_cali_curve_fitting_config_t;

struct adc_cali_scheme_t {
    int atten;
    int bitwidth;
    float vref_mv;  // Full-scale reference voltage in mV
};

// Line fitting scheme
inline esp_err_t adc_cali_create_scheme_line_fitting(
    const adc_cali_line_fitting_config_t* config,
    adc_cali_handle_t* ret_handle) {
    auto h = new adc_cali_scheme_t();
    h->atten = config->atten;
    h->bitwidth = config->bitwidth ? config->bitwidth : 12;
    // Approximate vref based on attenuation
    switch (config->atten) {
        case 0: h->vref_mv = 750.0f; break;
        case 1: h->vref_mv = 1050.0f; break;
        case 2: h->vref_mv = 1300.0f; break;
        case 3: h->vref_mv = 2450.0f; break;
        default: h->vref_mv = 2450.0f; break;
    }
    *ret_handle = h;
    fprintf(stderr, "[esp32emu] ADC calibration (line fitting) created: atten=%d, bits=%d\n",
            h->atten, h->bitwidth);
    return ESP_OK;
}

// Curve fitting scheme
inline esp_err_t adc_cali_create_scheme_curve_fitting(
    const adc_cali_curve_fitting_config_t* config,
    adc_cali_handle_t* ret_handle) {
    auto h = new adc_cali_scheme_t();
    h->atten = config->atten;
    h->bitwidth = config->bitwidth ? config->bitwidth : 12;
    switch (config->atten) {
        case 0: h->vref_mv = 750.0f; break;
        case 1: h->vref_mv = 1050.0f; break;
        case 2: h->vref_mv = 1300.0f; break;
        case 3: h->vref_mv = 2450.0f; break;
        default: h->vref_mv = 2450.0f; break;
    }
    *ret_handle = h;
    fprintf(stderr, "[esp32emu] ADC calibration (curve fitting) created: atten=%d, bits=%d\n",
            h->atten, h->bitwidth);
    return ESP_OK;
}

// Convert raw ADC reading to millivolts
inline esp_err_t adc_cali_raw_to_voltage(adc_cali_handle_t handle,
                                          int raw, int* voltage_mv) {
    int max_raw = (1 << handle->bitwidth) - 1;
    *voltage_mv = (int)((float)raw / max_raw * handle->vref_mv);
    return ESP_OK;
}

inline esp_err_t adc_cali_delete_scheme_line_fitting(adc_cali_handle_t handle) {
    delete handle;
    return ESP_OK;
}

inline esp_err_t adc_cali_delete_scheme_curve_fitting(adc_cali_handle_t handle) {
    delete handle;
    return ESP_OK;
}

#ifdef __cplusplus
}
#endif
