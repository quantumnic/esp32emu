#pragma once
// esp32emu: Pulse Counter (PCNT) driver mock
// Emulates ESP-IDF PCNT peripheral for rotary encoders, frequency counters, etc.

#include <cstdint>
#include <cstring>
#include <functional>

#ifdef __cplusplus
extern "C" {
#endif

#define ESP_OK 0

typedef int esp_err_t;

typedef enum { PCNT_UNIT_0 = 0, PCNT_UNIT_1, PCNT_UNIT_2, PCNT_UNIT_3, PCNT_UNIT_MAX } pcnt_unit_t;
typedef enum { PCNT_CHANNEL_0 = 0, PCNT_CHANNEL_1, PCNT_CHANNEL_MAX } pcnt_channel_t;
typedef enum { PCNT_COUNT_DIS = 0, PCNT_COUNT_INC, PCNT_COUNT_DEC } pcnt_count_mode_t;
typedef enum { PCNT_MODE_KEEP = 0, PCNT_MODE_REVERSE, PCNT_MODE_DISABLE } pcnt_ctrl_mode_t;
typedef enum {
    PCNT_EVT_THRES_0 = (1 << 0),
    PCNT_EVT_THRES_1 = (1 << 1),
    PCNT_EVT_L_LIM   = (1 << 2),
    PCNT_EVT_H_LIM   = (1 << 3),
    PCNT_EVT_ZERO    = (1 << 4),
} pcnt_evt_type_t;

typedef struct {
    int pulse_gpio_num;
    int ctrl_gpio_num;
    pcnt_count_mode_t pos_mode;
    pcnt_count_mode_t neg_mode;
    pcnt_ctrl_mode_t lctrl_mode;
    pcnt_ctrl_mode_t hctrl_mode;
    pcnt_channel_t channel;
    pcnt_unit_t unit;
    int16_t counter_h_lim;
    int16_t counter_l_lim;
} pcnt_config_t;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
namespace esp32emu {
namespace pcnt {

struct UnitState {
    bool configured = false;
    bool filter_enabled = false;
    uint16_t filter_value = 0;
    int16_t count = 0;
    int16_t h_lim = 0;
    int16_t l_lim = 0;
    pcnt_count_mode_t pos_mode = PCNT_COUNT_DIS;
    pcnt_count_mode_t neg_mode = PCNT_COUNT_DIS;
};

inline UnitState& getUnit(pcnt_unit_t unit) {
    static UnitState units[PCNT_UNIT_MAX];
    return units[unit];
}

// Test helper: simulate pulses
inline void injectPulses(pcnt_unit_t unit, int count) {
    auto& u = getUnit(unit);
    for (int i = 0; i < count; i++) {
        if (u.pos_mode == PCNT_COUNT_INC) u.count++;
        else if (u.pos_mode == PCNT_COUNT_DEC) u.count--;
        if (u.h_lim != 0 && u.count >= u.h_lim) u.count = 0;
        if (u.l_lim != 0 && u.count <= u.l_lim) u.count = 0;
    }
}

} // namespace pcnt
} // namespace esp32emu

inline esp_err_t pcnt_unit_config(const pcnt_config_t* config) {
    if (!config) return -1;
    auto& u = esp32emu::pcnt::getUnit(config->unit);
    u.configured = true;
    u.pos_mode = config->pos_mode;
    u.neg_mode = config->neg_mode;
    u.h_lim = config->counter_h_lim;
    u.l_lim = config->counter_l_lim;
    u.count = 0;
    return ESP_OK;
}

inline esp_err_t pcnt_get_counter_value(pcnt_unit_t unit, int16_t* count) {
    if (!count || unit >= PCNT_UNIT_MAX) return -1;
    *count = esp32emu::pcnt::getUnit(unit).count;
    return ESP_OK;
}

inline esp_err_t pcnt_counter_pause(pcnt_unit_t unit) {
    if (unit >= PCNT_UNIT_MAX) return -1;
    return ESP_OK;
}

inline esp_err_t pcnt_counter_resume(pcnt_unit_t unit) {
    if (unit >= PCNT_UNIT_MAX) return -1;
    return ESP_OK;
}

inline esp_err_t pcnt_counter_clear(pcnt_unit_t unit) {
    if (unit >= PCNT_UNIT_MAX) return -1;
    esp32emu::pcnt::getUnit(unit).count = 0;
    return ESP_OK;
}

inline esp_err_t pcnt_set_filter_value(pcnt_unit_t unit, uint16_t filter_val) {
    if (unit >= PCNT_UNIT_MAX) return -1;
    esp32emu::pcnt::getUnit(unit).filter_value = filter_val;
    return ESP_OK;
}

inline esp_err_t pcnt_filter_enable(pcnt_unit_t unit) {
    if (unit >= PCNT_UNIT_MAX) return -1;
    esp32emu::pcnt::getUnit(unit).filter_enabled = true;
    return ESP_OK;
}

inline esp_err_t pcnt_filter_disable(pcnt_unit_t unit) {
    if (unit >= PCNT_UNIT_MAX) return -1;
    esp32emu::pcnt::getUnit(unit).filter_enabled = false;
    return ESP_OK;
}

inline esp_err_t pcnt_set_event_value(pcnt_unit_t unit, pcnt_evt_type_t /*evt*/, int16_t /*value*/) {
    if (unit >= PCNT_UNIT_MAX) return -1;
    return ESP_OK;
}

inline esp_err_t pcnt_event_enable(pcnt_unit_t unit, pcnt_evt_type_t /*evt*/) {
    if (unit >= PCNT_UNIT_MAX) return -1;
    return ESP_OK;
}

inline esp_err_t pcnt_event_disable(pcnt_unit_t unit, pcnt_evt_type_t /*evt*/) {
    if (unit >= PCNT_UNIT_MAX) return -1;
    return ESP_OK;
}
#endif
