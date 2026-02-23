// ESP-IDF ADC Continuous (DMA) mode example
// High-speed multi-channel ADC sampling

#include <Arduino.h>
#include <esp_adc_continuous.h>

adc_continuous_handle_t adc_handle = nullptr;

void setup() {
    Serial.begin(115200);
    Serial.println("ADC Continuous Mode Sampling");
    Serial.println("============================");

    // Create handle
    adc_continuous_handle_cfg_t handle_cfg = {};
    handle_cfg.max_store_buf_size = 1024;
    handle_cfg.conv_frame_size = 256;
    adc_continuous_new_handle(&handle_cfg, &adc_handle);

    // Configure two channels
    adc_digi_pattern_config_t patterns[2] = {};
    patterns[0].channel = 0;  // ADC1_CH0 (GPIO36)
    patterns[0].unit = 0;
    patterns[0].atten = 3;    // 11dB attenuation
    patterns[0].bit_width = 12;
    patterns[1].channel = 3;  // ADC1_CH3 (GPIO39)
    patterns[1].unit = 0;
    patterns[1].atten = 3;
    patterns[1].bit_width = 12;

    adc_continuous_config_t cfg = {};
    cfg.pattern_num = 2;
    cfg.adc_pattern = patterns;
    cfg.sample_freq_hz = 20000;
    cfg.conv_mode = ADC_CONV_SINGLE_UNIT_1;
    cfg.format = ADC_CONV_ELAPSE_FORMAT;
    adc_continuous_config(adc_handle, &cfg);

    adc_continuous_start(adc_handle);
    Serial.println("ADC started at 20kHz sampling rate");
}

void loop() {
    uint8_t buf[256];
    uint32_t out_len = 0;

    esp_err_t ret = adc_continuous_read(adc_handle, buf, sizeof(buf), &out_len, 100);
    if (ret == ESP_OK && out_len > 0) {
        int samples = out_len / sizeof(adc_digi_output_data_t);
        Serial.print("Read ");
        Serial.print(samples);
        Serial.println(" samples:");

        for (int i = 0; i < samples && i < 4; i++) {
            adc_digi_output_data_t *d = (adc_digi_output_data_t*)(buf + i * sizeof(adc_digi_output_data_t));
            Serial.print("  CH");
            Serial.print(d->channel);
            Serial.print(": ");
            Serial.print(d->val);
            float voltage = d->val * 3.3f / 4095.0f;
            Serial.print(" (");
            Serial.print(voltage, 2);
            Serial.println("V)");
        }
    }

    delay(1000);
}
