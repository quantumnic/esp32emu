// ADC Oneshot Read example — new ESP-IDF ADC API
#include "Arduino.h"
#include "driver/adc_oneshot.h"

adc_oneshot_unit_handle_t adc1_handle = nullptr;

void setup() {
    Serial.begin(115200);
    Serial.println("ADC Oneshot example starting...");

    adc_oneshot_unit_init_cfg_t init_cfg = { .unit_id = ADC_UNIT_1 };
    adc_oneshot_new_unit(&init_cfg, &adc1_handle);

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_12_ONESHOT,
        .atten = ADC_ATTEN_DB_12_ONESHOT
    };
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_0_ONESHOT, &chan_cfg);
    adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_3_ONESHOT, &chan_cfg);

    // Inject test values
    esp32emu_adc_oneshot::setRawValue(0, ADC_CHANNEL_0_ONESHOT, 1500);
    esp32emu_adc_oneshot::setRawValue(0, ADC_CHANNEL_3_ONESHOT, 3200);

    Serial.println("ADC configured.");
}

void loop() {
    int raw0 = 0, raw3 = 0;
    adc_oneshot_read(adc1_handle, ADC_CHANNEL_0_ONESHOT, &raw0);
    adc_oneshot_read(adc1_handle, ADC_CHANNEL_3_ONESHOT, &raw3);

    float voltage0 = raw0 * 3.3f / 4095.0f;
    float voltage3 = raw3 * 3.3f / 4095.0f;

    Serial.print("CH0: "); Serial.print(raw0); Serial.print(" ("); Serial.print(voltage0); Serial.println(" V)");
    Serial.print("CH3: "); Serial.print(raw3); Serial.print(" ("); Serial.print(voltage3); Serial.println(" V)");

    delay(1000);
}
