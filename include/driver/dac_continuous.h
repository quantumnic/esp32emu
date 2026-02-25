#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <vector>

// ESP-IDF DAC continuous (DMA) output driver mock
// Supports continuous waveform generation via DMA-like buffer writes

#ifdef __cplusplus
extern "C" {
#endif

typedef void* dac_continuous_handle_t;

typedef enum {
    DAC_CHAN_0 = 0,
    DAC_CHAN_1 = 1,
} dac_channel_t;

typedef enum {
    DAC_DIGI_CLK_SRC_DEFAULT = 0,
    DAC_DIGI_CLK_SRC_APB = 1,
} dac_continuous_digi_clk_src_t;

typedef struct {
    dac_channel_t chan_mask;
    uint32_t desc_num;
    size_t buf_size;
    uint32_t freq_hz;
    int offset;
    dac_continuous_digi_clk_src_t clk_src;
    bool chan_mode; // true = alternating
} dac_continuous_config_t;

typedef struct {
    void (*on_convert_done)(void* arg);
    void* arg;
} dac_event_callbacks_t;

// Internal state for mock
namespace esp32emu_dac_cont {
    struct DacState {
        bool initialized = false;
        bool enabled = false;
        dac_continuous_config_t config = {};
        std::vector<uint8_t> last_written;
        size_t total_bytes_written = 0;
        bool cyclic = false;
    };
    inline DacState& state() {
        static DacState s;
        return s;
    }
}

inline int dac_continuous_new_channels(const dac_continuous_config_t* cfg, dac_continuous_handle_t* handle) {
    if (!cfg || !handle) return -1;
    auto& s = esp32emu_dac_cont::state();
    s.config = *cfg;
    s.initialized = true;
    s.enabled = false;
    *handle = (void*)0x1;
    fprintf(stderr, "[esp32emu] DAC continuous: channel %d, freq %u Hz, buf %zu\n",
            cfg->chan_mask, cfg->freq_hz, cfg->buf_size);
    return 0; // ESP_OK
}

inline int dac_continuous_enable(dac_continuous_handle_t handle) {
    if (!handle) return -1;
    esp32emu_dac_cont::state().enabled = true;
    return 0;
}

inline int dac_continuous_disable(dac_continuous_handle_t handle) {
    if (!handle) return -1;
    esp32emu_dac_cont::state().enabled = false;
    return 0;
}

inline int dac_continuous_del_channels(dac_continuous_handle_t handle) {
    if (!handle) return -1;
    auto& s = esp32emu_dac_cont::state();
    s.initialized = false;
    s.enabled = false;
    s.last_written.clear();
    s.total_bytes_written = 0;
    return 0;
}

inline int dac_continuous_write(dac_continuous_handle_t handle, uint8_t* buf, size_t buf_size, size_t* bytes_loaded, int timeout_ms) {
    (void)timeout_ms;
    if (!handle || !buf) return -1;
    auto& s = esp32emu_dac_cont::state();
    s.last_written.assign(buf, buf + buf_size);
    s.total_bytes_written += buf_size;
    if (bytes_loaded) *bytes_loaded = buf_size;
    return 0;
}

inline int dac_continuous_write_cyclically(dac_continuous_handle_t handle, uint8_t* buf, size_t buf_size, size_t* bytes_loaded) {
    if (!handle || !buf) return -1;
    auto& s = esp32emu_dac_cont::state();
    s.last_written.assign(buf, buf + buf_size);
    s.total_bytes_written += buf_size;
    s.cyclic = true;
    if (bytes_loaded) *bytes_loaded = buf_size;
    return 0;
}

inline int dac_continuous_register_event_callback(dac_continuous_handle_t handle, const dac_event_callbacks_t* cbs) {
    (void)handle; (void)cbs;
    return 0;
}

#ifdef __cplusplus
}
#endif
