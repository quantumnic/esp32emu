// esp32emu — test_adc: ADC driver mock test
#include "driver/adc.h"
#include <cassert>
#include <cstdio>

int main() {
    // Config
    assert(adc1_config_width(ADC_WIDTH_BIT_12) == ESP_OK);
    assert(adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11) == ESP_OK);

    // Random value in range
    int raw = adc1_get_raw(ADC1_CHANNEL_0);
    assert(raw >= 0 && raw <= 4095);

    // Inject specific value
    esp32emu_adc_test::injectADC1(ADC1_CHANNEL_0, 2048);
    assert(adc1_get_raw(ADC1_CHANNEL_0) == 2048);
    assert(adc1_get_raw(ADC1_CHANNEL_0) == 2048); // stable

    // ADC2
    assert(adc2_config_channel_atten(ADC2_CHANNEL_0, ADC_ATTEN_DB_11) == ESP_OK);
    esp32emu_adc_test::injectADC2(ADC2_CHANNEL_0, 1000);
    int raw2;
    assert(adc2_get_raw(ADC2_CHANNEL_0, ADC_WIDTH_BIT_12, &raw2) == ESP_OK);
    assert(raw2 == 1000);

    // Different width
    assert(adc1_config_width(ADC_WIDTH_BIT_10) == ESP_OK);
    esp32emu_adc_test::clearInjections();
    raw = adc1_get_raw(ADC1_CHANNEL_0);
    assert(raw >= 0 && raw <= 1023);

    printf("test_adc: all assertions passed\n");
    return 0;
}
