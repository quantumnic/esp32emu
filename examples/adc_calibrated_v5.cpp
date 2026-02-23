// ESP-IDF v5 ADC calibration — read calibrated voltage
#include <Arduino.h>
#include <esp_adc_cali.h>

void setup() {
    Serial.begin(115200);

    adc_cali_line_fitting_config_t cali_cfg = {};
    cali_cfg.unit_id = ADC_UNIT_1;
    cali_cfg.atten = ADC_ATTEN_DB_12;
    cali_cfg.bitwidth = ADC_BITWIDTH_12;

    adc_cali_handle_t cali_handle;
    ESP_ERROR_CHECK(adc_cali_create_scheme_line_fitting(&cali_cfg, &cali_handle));

    // Simulate reading raw ADC values
    int raw_values[] = {0, 1024, 2048, 3072, 4095};
    for (int raw : raw_values) {
        int voltage_mv = 0;
        adc_cali_raw_to_voltage(cali_handle, raw, &voltage_mv);
        Serial.print("Raw: "); Serial.print(raw);
        Serial.print(" -> "); Serial.print(voltage_mv);
        Serial.println(" mV");
    }

    adc_cali_delete_scheme_line_fitting(cali_handle);
}

void loop() {
    delay(1000);
}
