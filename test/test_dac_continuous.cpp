#include "Arduino.h"
#include "driver/dac_continuous.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    // Test 1: Create and delete channels
    {
        dac_continuous_handle_t handle = nullptr;
        dac_continuous_config_t cfg = {};
        cfg.chan_mask = DAC_CHAN_0;
        cfg.freq_hz = 8000;
        cfg.buf_size = 1024;
        cfg.desc_num = 8;
        assert(dac_continuous_new_channels(&cfg, &handle) == 0);
        assert(handle != nullptr);
        assert(esp32emu_dac_cont::state().initialized);
        assert(dac_continuous_del_channels(handle) == 0);
        assert(!esp32emu_dac_cont::state().initialized);
    }

    // Test 2: Enable/disable
    {
        dac_continuous_handle_t handle;
        dac_continuous_config_t cfg = {};
        cfg.chan_mask = DAC_CHAN_1;
        cfg.freq_hz = 44100;
        cfg.buf_size = 2048;
        dac_continuous_new_channels(&cfg, &handle);
        assert(!esp32emu_dac_cont::state().enabled);
        assert(dac_continuous_enable(handle) == 0);
        assert(esp32emu_dac_cont::state().enabled);
        assert(dac_continuous_disable(handle) == 0);
        assert(!esp32emu_dac_cont::state().enabled);
        dac_continuous_del_channels(handle);
    }

    // Test 3: Write data
    {
        dac_continuous_handle_t handle;
        dac_continuous_config_t cfg = {};
        cfg.chan_mask = DAC_CHAN_0;
        cfg.freq_hz = 8000;
        cfg.buf_size = 256;
        dac_continuous_new_channels(&cfg, &handle);
        dac_continuous_enable(handle);

        uint8_t buf[64];
        for (int i = 0; i < 64; i++) buf[i] = (uint8_t)(128 + 127 * sin(2.0 * M_PI * i / 64));
        size_t loaded = 0;
        assert(dac_continuous_write(handle, buf, 64, &loaded, 1000) == 0);
        assert(loaded == 64);
        assert(esp32emu_dac_cont::state().total_bytes_written == 64);
        assert(esp32emu_dac_cont::state().last_written.size() == 64);
        assert(esp32emu_dac_cont::state().last_written[0] == buf[0]);

        dac_continuous_del_channels(handle);
    }

    // Test 4: Cyclic write
    {
        dac_continuous_handle_t handle;
        dac_continuous_config_t cfg = {};
        cfg.chan_mask = DAC_CHAN_0;
        cfg.freq_hz = 16000;
        cfg.buf_size = 128;
        dac_continuous_new_channels(&cfg, &handle);
        dac_continuous_enable(handle);

        uint8_t buf[32];
        for (int i = 0; i < 32; i++) buf[i] = i * 8;
        size_t loaded = 0;
        assert(dac_continuous_write_cyclically(handle, buf, 32, &loaded) == 0);
        assert(loaded == 32);
        assert(esp32emu_dac_cont::state().cyclic);

        dac_continuous_del_channels(handle);
    }

    // Test 5: Null handle errors
    {
        assert(dac_continuous_enable(nullptr) == -1);
        assert(dac_continuous_disable(nullptr) == -1);
        assert(dac_continuous_write(nullptr, nullptr, 0, nullptr, 0) == -1);
        assert(dac_continuous_del_channels(nullptr) == -1);
    }

    // Test 6: Config preserved
    {
        dac_continuous_handle_t handle;
        dac_continuous_config_t cfg = {};
        cfg.chan_mask = DAC_CHAN_1;
        cfg.freq_hz = 22050;
        cfg.buf_size = 512;
        cfg.desc_num = 4;
        dac_continuous_new_channels(&cfg, &handle);
        assert(esp32emu_dac_cont::state().config.freq_hz == 22050);
        assert(esp32emu_dac_cont::state().config.chan_mask == DAC_CHAN_1);
        assert(esp32emu_dac_cont::state().config.buf_size == 512);
        dac_continuous_del_channels(handle);
    }

    // Test 7: Event callback registration (no-op but should succeed)
    {
        dac_continuous_handle_t handle;
        dac_continuous_config_t cfg = {};
        cfg.freq_hz = 8000;
        dac_continuous_new_channels(&cfg, &handle);
        dac_event_callbacks_t cbs = {};
        assert(dac_continuous_register_event_callback(handle, &cbs) == 0);
        dac_continuous_del_channels(handle);
    }

    printf("test_dac_continuous: all assertions passed\n");
    return 0;
}
