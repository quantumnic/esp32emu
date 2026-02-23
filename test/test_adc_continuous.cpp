#include "Arduino.h"
#include "esp_adc_continuous.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    esp32emu_adc_cont::reset();

    // Create handle
    adc_continuous_handle_t handle = nullptr;
    adc_continuous_handle_cfg_t handle_cfg = {};
    handle_cfg.max_store_buf_size = 1024;
    handle_cfg.conv_frame_size = 256;
    assert(adc_continuous_new_handle(&handle_cfg, &handle) == ESP_OK);
    assert(handle != nullptr);

    // Configure
    adc_digi_pattern_config_t patterns[2] = {};
    patterns[0].channel = 0;
    patterns[0].unit = 0;
    patterns[0].atten = 3;
    patterns[0].bit_width = 12;
    patterns[1].channel = 3;
    patterns[1].unit = 0;
    patterns[1].atten = 3;
    patterns[1].bit_width = 12;

    adc_continuous_config_t cfg = {};
    cfg.pattern_num = 2;
    cfg.adc_pattern = patterns;
    cfg.sample_freq_hz = 20000;
    cfg.conv_mode = ADC_CONV_SINGLE_UNIT_1;
    cfg.format = ADC_CONV_ELAPSE_FORMAT;
    assert(adc_continuous_config(handle, &cfg) == ESP_OK);
    assert(test_adc_continuous_get_sample_freq() == 20000);

    // Register callbacks (just ensure it doesn't fail)
    adc_continuous_evt_cbs_t cbs = {};
    assert(adc_continuous_register_event_callbacks(handle, &cbs, nullptr) == ESP_OK);

    // Start
    assert(adc_continuous_start(handle) == ESP_OK);
    assert(test_adc_continuous_is_started());

    // Read default data
    uint8_t buf[256];
    uint32_t out_len = 0;
    assert(adc_continuous_read(handle, buf, sizeof(buf), &out_len, 0) == ESP_OK);
    assert(out_len > 0);

    // Read first result
    adc_digi_output_data_t *result = (adc_digi_output_data_t*)buf;
    assert(result->val == 2048); // mid-scale default
    assert(result->channel == 0);

    // Inject custom data
    adc_digi_output_data_t custom;
    custom.val = 3000;
    custom.channel = 5;
    custom.unit = 0;
    test_adc_continuous_inject((uint8_t*)&custom, sizeof(custom));
    assert(adc_continuous_read(handle, buf, sizeof(buf), &out_len, 0) == ESP_OK);
    result = (adc_digi_output_data_t*)buf;
    assert(result->val == 3000);
    assert(result->channel == 5);

    // Stop
    assert(adc_continuous_stop(handle) == ESP_OK);
    assert(!test_adc_continuous_is_started());

    // Read while stopped should fail
    assert(adc_continuous_read(handle, buf, sizeof(buf), &out_len, 0) == ESP_ERR_INVALID_STATE);

    // Deinit
    assert(adc_continuous_deinit(handle) == ESP_OK);

    printf("test_adc_continuous: all assertions passed\n");
    return 0;
}
