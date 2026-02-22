#include "esp_adc_oneshot.h"
#include <cassert>
#include <cstdio>

int main() {
    _test_adc_oneshot_reset();

    // Test 1: Create and delete unit
    {
        adc_oneshot_unit_handle_t handle = nullptr;
        adc_oneshot_unit_init_cfg_t cfg = {.unit_id = 0, .clk_src = 0, .ulp_mode = 0};
        assert(adc_oneshot_new_unit(&cfg, &handle) == ESP_OK);
        assert(handle != nullptr);
        assert(adc_oneshot_del_unit(handle) == ESP_OK);
    }

    // Test 2: Configure channel and read
    {
        adc_oneshot_unit_handle_t handle;
        adc_oneshot_unit_init_cfg_t cfg = {.unit_id = 0, .clk_src = 0, .ulp_mode = 0};
        adc_oneshot_new_unit(&cfg, &handle);

        adc_oneshot_chan_cfg_t chan_cfg = {.atten = ADC_ATTEN_DB_12, .bitwidth = ADC_BITWIDTH_12};
        assert(adc_oneshot_config_channel(handle, 0, &chan_cfg) == ESP_OK);

        int raw = 0;
        assert(adc_oneshot_read(handle, 0, &raw) == ESP_OK);
        assert(raw == 2048);  // default

        adc_oneshot_del_unit(handle);
    }

    // Test 3: Inject test values
    {
        _test_adc_oneshot_set_value(0, 3, 3000);
        adc_oneshot_unit_handle_t handle;
        adc_oneshot_unit_init_cfg_t cfg = {.unit_id = 0, .clk_src = 0, .ulp_mode = 0};
        adc_oneshot_new_unit(&cfg, &handle);

        int raw = 0;
        adc_oneshot_read(handle, 3, &raw);
        assert(raw == 3000);

        adc_oneshot_del_unit(handle);
    }

    // Test 4: Unit 2
    {
        _test_adc_oneshot_set_value(1, 0, 1000);
        adc_oneshot_unit_handle_t handle;
        adc_oneshot_unit_init_cfg_t cfg = {.unit_id = 1, .clk_src = 0, .ulp_mode = 0};
        adc_oneshot_new_unit(&cfg, &handle);

        int raw = 0;
        adc_oneshot_read(handle, 0, &raw);
        assert(raw == 1000);

        adc_oneshot_del_unit(handle);
    }

    // Test 5: Invalid args
    {
        assert(adc_oneshot_new_unit(nullptr, nullptr) == ESP_ERR_INVALID_ARG);
        assert(adc_oneshot_read(nullptr, 0, nullptr) == ESP_ERR_INVALID_ARG);
    }

    _test_adc_oneshot_reset();
    printf("test_adc_oneshot: all assertions passed\n");
    return 0;
}
