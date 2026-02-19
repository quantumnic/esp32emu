// Example: ADC reading — reads analog sensor on ADC1 channel 0
#include <Arduino.h>
#include "driver/adc.h"
#include <cstdio>

void setup() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);
    printf("ADC configured: 12-bit, 11dB attenuation\n");
}

void loop() {
    int raw = adc1_get_raw(ADC1_CHANNEL_0);
    float voltage = raw * 3.3f / 4095.0f;
    printf("ADC raw: %d, voltage: %.2fV\n", raw, voltage);
    delay(500);
}
