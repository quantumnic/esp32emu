#include <Arduino.h>
#include "esp_adc_oneshot.h"

void setup() {
    Serial.begin(115200);
    Serial.println("ESP-IDF ADC Oneshot Driver Example");

    adc_oneshot_unit_handle_t adc_handle;
    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = 0,
        .clk_src = 0,
        .ulp_mode = 0,
    };
    esp_err_t err = adc_oneshot_new_unit(&init_cfg, &adc_handle);
    if (err != ESP_OK) {
        Serial.println("Failed to create ADC unit!");
        return;
    }

    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    adc_oneshot_config_channel(adc_handle, 0, &chan_cfg);
    adc_oneshot_config_channel(adc_handle, 3, &chan_cfg);

    Serial.println("ADC configured (channels 0, 3)");

    for (int i = 0; i < 5; i++) {
        int raw0, raw3;
        adc_oneshot_read(adc_handle, 0, &raw0);
        adc_oneshot_read(adc_handle, 3, &raw3);
        Serial.printf("CH0: %d  CH3: %d\n", raw0, raw3);
        delay(500);
    }

    adc_oneshot_del_unit(adc_handle);
    Serial.println("ADC unit deleted");
}

void loop() {
    delay(1000);
}
