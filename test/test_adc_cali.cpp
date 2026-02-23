#include "esp_adc_cali.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    // Test line fitting scheme
    adc_cali_line_fitting_config_t line_cfg = {};
    line_cfg.unit_id = ADC_UNIT_1;
    line_cfg.atten = ADC_ATTEN_DB_12;
    line_cfg.bitwidth = ADC_BITWIDTH_12;

    adc_cali_handle_t handle = nullptr;
    assert(adc_cali_create_scheme_line_fitting(&line_cfg, &handle) == ESP_OK);
    assert(handle != nullptr);

    // Convert raw to voltage
    int voltage = 0;
    // Mid-range raw (2048 out of 4095) should give ~half of 2450mV
    assert(adc_cali_raw_to_voltage(handle, 2048, &voltage) == ESP_OK);
    assert(abs(voltage - 1225) < 5);  // ~1225mV

    // Max raw
    assert(adc_cali_raw_to_voltage(handle, 4095, &voltage) == ESP_OK);
    assert(abs(voltage - 2450) < 5);

    // Zero raw
    assert(adc_cali_raw_to_voltage(handle, 0, &voltage) == ESP_OK);
    assert(voltage == 0);

    assert(adc_cali_delete_scheme_line_fitting(handle) == ESP_OK);

    // Test curve fitting scheme
    adc_cali_curve_fitting_config_t curve_cfg = {};
    curve_cfg.unit_id = ADC_UNIT_1;
    curve_cfg.atten = 0;  // ADC_ATTEN_DB_0
    curve_cfg.bitwidth = 12;

    adc_cali_handle_t handle2 = nullptr;
    assert(adc_cali_create_scheme_curve_fitting(&curve_cfg, &handle2) == ESP_OK);

    // DB_0 has 750mV range
    assert(adc_cali_raw_to_voltage(handle2, 4095, &voltage) == ESP_OK);
    assert(abs(voltage - 750) < 5);

    assert(adc_cali_delete_scheme_curve_fitting(handle2) == ESP_OK);

    printf("test_adc_cali: all assertions passed\n");
    return 0;
}
