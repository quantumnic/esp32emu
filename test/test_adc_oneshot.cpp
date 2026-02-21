#include "driver/adc_oneshot.h"
#include <cassert>
#include <cstdio>

int main() {
    // Test 1: Create and read with default value
    {
        adc_oneshot_unit_handle_t handle = nullptr;
        adc_oneshot_unit_init_cfg_t cfg = { .unit_id = ADC_UNIT_1 };
        assert(adc_oneshot_new_unit(&cfg, &handle) == ESP_OK);
        assert(handle != nullptr);

        adc_oneshot_chan_cfg_t ch_cfg = {
            .bitwidth = ADC_BITWIDTH_12_ONESHOT,
            .atten = ADC_ATTEN_DB_12_ONESHOT
        };
        assert(adc_oneshot_config_channel(handle, ADC_CHANNEL_0_ONESHOT, &ch_cfg) == ESP_OK);

        int raw = 0;
        assert(adc_oneshot_read(handle, ADC_CHANNEL_0_ONESHOT, &raw) == ESP_OK);
        assert(raw == 2048); // default mid-range

        assert(adc_oneshot_del_unit(handle) == ESP_OK);
    }

    // Test 2: Inject values
    {
        adc_oneshot_unit_handle_t handle = nullptr;
        adc_oneshot_unit_init_cfg_t cfg = { .unit_id = ADC_UNIT_1 };
        adc_oneshot_new_unit(&cfg, &handle);

        adc_oneshot_chan_cfg_t ch_cfg = {
            .bitwidth = ADC_BITWIDTH_12_ONESHOT,
            .atten = ADC_ATTEN_DB_12_ONESHOT
        };
        adc_oneshot_config_channel(handle, ADC_CHANNEL_3_ONESHOT, &ch_cfg);

        esp32emu_adc_oneshot::setRawValue(0, ADC_CHANNEL_3_ONESHOT, 3500);
        int raw = 0;
        adc_oneshot_read(handle, ADC_CHANNEL_3_ONESHOT, &raw);
        assert(raw == 3500);

        esp32emu_adc_oneshot::setRawValue(0, ADC_CHANNEL_3_ONESHOT, 0);
        adc_oneshot_read(handle, ADC_CHANNEL_3_ONESHOT, &raw);
        assert(raw == 0);

        adc_oneshot_del_unit(handle);
    }

    // Test 3: Multiple units
    {
        adc_oneshot_unit_handle_t h1 = nullptr, h2 = nullptr;
        adc_oneshot_unit_init_cfg_t cfg1 = { .unit_id = ADC_UNIT_1 };
        adc_oneshot_unit_init_cfg_t cfg2 = { .unit_id = ADC_UNIT_2 };
        adc_oneshot_new_unit(&cfg1, &h1);
        adc_oneshot_new_unit(&cfg2, &h2);
        assert(h1 != h2);

        esp32emu_adc_oneshot::setRawValue(0, 0, 1000);
        esp32emu_adc_oneshot::setRawValue(1, 0, 3000);

        int r1 = 0, r2 = 0;
        adc_oneshot_read(h1, 0, &r1);
        adc_oneshot_read(h2, 0, &r2);
        assert(r1 == 1000);
        assert(r2 == 3000);

        adc_oneshot_del_unit(h1);
        adc_oneshot_del_unit(h2);
    }

    // Test 4: Invalid args
    {
        assert(adc_oneshot_read(nullptr, 0, nullptr) == ESP_ERR_INVALID_ARG);
        assert(adc_oneshot_del_unit(nullptr) == ESP_ERR_INVALID_ARG);
    }

    printf("test_adc_oneshot: all assertions passed\n");
    return 0;
}
