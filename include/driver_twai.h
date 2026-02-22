#pragma once
// esp32emu: TWAI (CAN bus) driver mock
#include <cstdint>
#include <cstdio>
#include <vector>
#include <cstring>

typedef enum {
    TWAI_STATE_STOPPED = 0,
    TWAI_STATE_RUNNING,
    TWAI_STATE_BUS_OFF,
    TWAI_STATE_RECOVERING
} twai_state_t;

typedef enum {
    TWAI_MODE_NORMAL = 0,
    TWAI_MODE_NO_ACK,
    TWAI_MODE_LISTEN_ONLY
} twai_mode_t;

typedef struct {
    twai_mode_t mode;
    int tx_gpio;
    int rx_gpio;
} twai_general_config_t;

typedef struct {
    uint32_t brp;
    uint8_t tseg_1;
    uint8_t tseg_2;
    uint8_t sjw;
    bool triple_sampling;
} twai_timing_config_t;

typedef struct {
    uint32_t acceptance_code;
    uint32_t acceptance_mask;
    bool single_filter;
} twai_filter_config_t;

typedef struct {
    uint32_t identifier;
    uint8_t data_length_code;
    uint8_t data[8];
    struct {
        uint32_t extd : 1;
        uint32_t rtr : 1;
        uint32_t ss : 1;
        uint32_t self : 1;
        uint32_t dlc_non_comp : 1;
    } flags;
} twai_message_t;

typedef struct {
    twai_state_t state;
    uint32_t msgs_to_tx;
    uint32_t msgs_to_rx;
    uint32_t tx_error_counter;
    uint32_t rx_error_counter;
    uint32_t tx_failed_count;
    uint32_t rx_missed_count;
    uint32_t arb_lost_count;
    uint32_t bus_error_count;
} twai_status_info_t;

// Predefined timing configs
#define TWAI_TIMING_CONFIG_25KBITS()   {.brp=128, .tseg_1=16, .tseg_2=8, .sjw=3, .triple_sampling=false}
#define TWAI_TIMING_CONFIG_50KBITS()   {.brp=80,  .tseg_1=15, .tseg_2=4, .sjw=3, .triple_sampling=false}
#define TWAI_TIMING_CONFIG_100KBITS()  {.brp=40,  .tseg_1=15, .tseg_2=4, .sjw=3, .triple_sampling=false}
#define TWAI_TIMING_CONFIG_125KBITS()  {.brp=32,  .tseg_1=15, .tseg_2=4, .sjw=3, .triple_sampling=false}
#define TWAI_TIMING_CONFIG_250KBITS()  {.brp=16,  .tseg_1=15, .tseg_2=4, .sjw=3, .triple_sampling=false}
#define TWAI_TIMING_CONFIG_500KBITS()  {.brp=8,   .tseg_1=15, .tseg_2=4, .sjw=3, .triple_sampling=false}
#define TWAI_TIMING_CONFIG_1MBITS()    {.brp=4,   .tseg_1=15, .tseg_2=4, .sjw=3, .triple_sampling=false}

#define TWAI_FILTER_CONFIG_ACCEPT_ALL() {.acceptance_code=0, .acceptance_mask=0xFFFFFFFF, .single_filter=true}

#define TWAI_GENERAL_CONFIG_DEFAULT(tx, rx, m) {.mode=(m), .tx_gpio=(tx), .rx_gpio=(rx)}

#ifndef ESP_OK
#define ESP_OK 0
#endif
#ifndef ESP_FAIL
#define ESP_FAIL -1
#endif
#ifndef ESP_ERR_INVALID_STATE
#define ESP_ERR_INVALID_STATE 0x103
#endif
#ifndef ESP_ERR_TIMEOUT
#define ESP_ERR_TIMEOUT 0x107
#endif

namespace esp32emu_twai {
    inline twai_state_t _state = TWAI_STATE_STOPPED;
    inline twai_general_config_t _g_config = {};
    inline twai_timing_config_t _t_config = {};
    inline twai_filter_config_t _f_config = {};
    inline std::vector<twai_message_t> _tx_queue;
    inline std::vector<twai_message_t> _rx_queue;
    inline uint32_t _tx_count = 0;
    inline uint32_t _rx_count = 0;

    // Test helpers
    inline void injectRxMessage(const twai_message_t& msg) { _rx_queue.push_back(msg); }
    inline const std::vector<twai_message_t>& getTxMessages() { return _tx_queue; }
    inline void reset() {
        _state = TWAI_STATE_STOPPED;
        _tx_queue.clear();
        _rx_queue.clear();
        _tx_count = 0;
        _rx_count = 0;
    }
}

inline int twai_driver_install(const twai_general_config_t* g, const twai_timing_config_t* t, const twai_filter_config_t* f) {
    if (esp32emu_twai::_state != TWAI_STATE_STOPPED) return ESP_ERR_INVALID_STATE;
    esp32emu_twai::_g_config = *g;
    esp32emu_twai::_t_config = *t;
    esp32emu_twai::_f_config = *f;
    printf("[esp32emu] TWAI: driver installed (mode=%d, tx=%d, rx=%d)\n", g->mode, g->tx_gpio, g->rx_gpio);
    return ESP_OK;
}

inline int twai_driver_uninstall() {
    if (esp32emu_twai::_state == TWAI_STATE_RUNNING) return ESP_ERR_INVALID_STATE;
    esp32emu_twai::_state = TWAI_STATE_STOPPED;
    printf("[esp32emu] TWAI: driver uninstalled\n");
    return ESP_OK;
}

inline int twai_start() {
    if (esp32emu_twai::_state == TWAI_STATE_RUNNING) return ESP_ERR_INVALID_STATE;
    esp32emu_twai::_state = TWAI_STATE_RUNNING;
    printf("[esp32emu] TWAI: started\n");
    return ESP_OK;
}

inline int twai_stop() {
    if (esp32emu_twai::_state != TWAI_STATE_RUNNING) return ESP_ERR_INVALID_STATE;
    esp32emu_twai::_state = TWAI_STATE_STOPPED;
    printf("[esp32emu] TWAI: stopped\n");
    return ESP_OK;
}

inline int twai_transmit(const twai_message_t* msg, uint32_t /*ticks_to_wait*/) {
    if (esp32emu_twai::_state != TWAI_STATE_RUNNING) return ESP_ERR_INVALID_STATE;
    esp32emu_twai::_tx_queue.push_back(*msg);
    esp32emu_twai::_tx_count++;
    return ESP_OK;
}

inline int twai_receive(twai_message_t* msg, uint32_t /*ticks_to_wait*/) {
    if (esp32emu_twai::_state != TWAI_STATE_RUNNING) return ESP_ERR_INVALID_STATE;
    if (esp32emu_twai::_rx_queue.empty()) return ESP_ERR_TIMEOUT;
    *msg = esp32emu_twai::_rx_queue.front();
    esp32emu_twai::_rx_queue.erase(esp32emu_twai::_rx_queue.begin());
    esp32emu_twai::_rx_count++;
    return ESP_OK;
}

inline int twai_get_status_info(twai_status_info_t* status) {
    status->state = esp32emu_twai::_state;
    status->msgs_to_tx = esp32emu_twai::_tx_queue.size();
    status->msgs_to_rx = esp32emu_twai::_rx_queue.size();
    status->tx_error_counter = 0;
    status->rx_error_counter = 0;
    status->tx_failed_count = 0;
    status->rx_missed_count = 0;
    status->arb_lost_count = 0;
    status->bus_error_count = 0;
    return ESP_OK;
}

inline int twai_initiate_recovery() {
    if (esp32emu_twai::_state != TWAI_STATE_BUS_OFF) return ESP_ERR_INVALID_STATE;
    esp32emu_twai::_state = TWAI_STATE_RECOVERING;
    printf("[esp32emu] TWAI: recovery initiated\n");
    return ESP_OK;
}
