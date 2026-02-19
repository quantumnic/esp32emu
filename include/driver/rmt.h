// driver/rmt.h — ESP32 RMT (Remote Control Transceiver) mock for esp32emu
#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

// RMT item: duration + level pairs
typedef struct {
    union {
        struct {
            uint32_t duration0 : 15;
            uint32_t level0    : 1;
            uint32_t duration1 : 15;
            uint32_t level1    : 1;
        };
        uint32_t val;
    };
} rmt_item32_t;

typedef enum {
    RMT_CHANNEL_0 = 0,
    RMT_CHANNEL_1,
    RMT_CHANNEL_2,
    RMT_CHANNEL_3,
    RMT_CHANNEL_4,
    RMT_CHANNEL_5,
    RMT_CHANNEL_6,
    RMT_CHANNEL_7,
    RMT_CHANNEL_MAX
} rmt_channel_t;

typedef enum {
    RMT_MODE_TX = 0,
    RMT_MODE_RX
} rmt_mode_t;

typedef enum {
    RMT_MEM_OWNER_TX = 0,
    RMT_MEM_OWNER_RX
} rmt_mem_owner_t;

typedef enum {
    RMT_CARRIER_LEVEL_LOW = 0,
    RMT_CARRIER_LEVEL_HIGH
} rmt_carrier_level_t;

typedef enum {
    RMT_IDLE_LEVEL_LOW = 0,
    RMT_IDLE_LEVEL_HIGH
} rmt_idle_level_t;

typedef struct {
    rmt_mode_t       rmt_mode;
    rmt_channel_t    channel;
    uint8_t          gpio_num;
    uint8_t          clk_div;
    uint8_t          mem_block_num;
    struct {
        uint32_t carrier_freq_hz;
        rmt_carrier_level_t carrier_level;
        rmt_idle_level_t idle_level;
        bool carrier_en;
        bool idle_output_en;
        bool loop_en;
    } tx_config;
    struct {
        uint16_t idle_threshold;
        uint8_t  filter_ticks_thresh;
        bool     filter_en;
    } rx_config;
} rmt_config_t;

#ifndef ESP_OK
#define ESP_OK 0
#define ESP_FAIL -1
#define ESP_ERR_INVALID_ARG 0x102
#define ESP_ERR_INVALID_STATE 0x103
#endif

// Internal state per channel
namespace rmt_internal {
    struct ChannelState {
        bool configured = false;
        bool installed = false;
        rmt_config_t config{};
        std::vector<rmt_item32_t> tx_log;
        std::vector<rmt_item32_t> rx_buffer;
        int tx_count = 0;
    };
    inline ChannelState& get(rmt_channel_t ch) {
        static ChannelState states[RMT_CHANNEL_MAX];
        return states[static_cast<int>(ch)];
    }
}

inline int rmt_config(const rmt_config_t* config) {
    if (!config || config->channel >= RMT_CHANNEL_MAX) return ESP_ERR_INVALID_ARG;
    auto& s = rmt_internal::get(config->channel);
    s.config = *config;
    s.configured = true;
    fprintf(stderr, "[esp32emu] RMT channel %d configured (GPIO %d, div=%d, mode=%s)\n",
            config->channel, config->gpio_num, config->clk_div,
            config->rmt_mode == RMT_MODE_TX ? "TX" : "RX");
    return ESP_OK;
}

inline int rmt_driver_install(rmt_channel_t channel, size_t /*rx_buf_size*/, int /*flags*/) {
    if (channel >= RMT_CHANNEL_MAX) return ESP_ERR_INVALID_ARG;
    auto& s = rmt_internal::get(channel);
    if (!s.configured) return ESP_ERR_INVALID_STATE;
    if (s.installed) return ESP_ERR_INVALID_STATE;
    s.installed = true;
    return ESP_OK;
}

inline int rmt_driver_uninstall(rmt_channel_t channel) {
    if (channel >= RMT_CHANNEL_MAX) return ESP_ERR_INVALID_ARG;
    auto& s = rmt_internal::get(channel);
    if (!s.installed) return ESP_ERR_INVALID_STATE;
    s.installed = false;
    return ESP_OK;
}

inline int rmt_write_items(rmt_channel_t channel, const rmt_item32_t* items, int num_items, bool /*wait_tx_done*/) {
    if (channel >= RMT_CHANNEL_MAX || !items) return ESP_ERR_INVALID_ARG;
    auto& s = rmt_internal::get(channel);
    if (!s.installed) return ESP_ERR_INVALID_STATE;
    for (int i = 0; i < num_items; i++) {
        s.tx_log.push_back(items[i]);
    }
    s.tx_count++;
    fprintf(stderr, "[esp32emu] RMT ch%d TX: %d items\n", channel, num_items);
    return ESP_OK;
}

inline int rmt_wait_tx_done(rmt_channel_t channel, uint32_t /*timeout_ms*/) {
    if (channel >= RMT_CHANNEL_MAX) return ESP_ERR_INVALID_ARG;
    auto& s = rmt_internal::get(channel);
    if (!s.installed) return ESP_ERR_INVALID_STATE;
    return ESP_OK;
}

// Test helpers
namespace rmt_test {
    inline void reset() {
        for (int i = 0; i < RMT_CHANNEL_MAX; i++) {
            rmt_internal::get(static_cast<rmt_channel_t>(i)) = {};
        }
    }
    inline void injectRxItems(rmt_channel_t ch, const rmt_item32_t* items, int count) {
        auto& s = rmt_internal::get(ch);
        s.rx_buffer.assign(items, items + count);
    }
    inline const std::vector<rmt_item32_t>& getTxLog(rmt_channel_t ch) {
        return rmt_internal::get(ch).tx_log;
    }
    inline int getTxCount(rmt_channel_t ch) {
        return rmt_internal::get(ch).tx_count;
    }
}
