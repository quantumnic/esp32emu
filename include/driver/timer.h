#pragma once
// esp32emu — ESP-IDF hardware timer group driver mock
// Emulates timer_init, timer_set_counter_value, timer_start, timer_get_counter_value, timer_set_alarm

#include <cstdint>
#include <cstdio>
#include <map>
#include <chrono>
#include <functional>

typedef int esp_err_t;
#ifndef ESP_OK
#define ESP_OK 0
#define ESP_FAIL -1
#define ESP_ERR_INVALID_ARG 0x102
#endif

typedef int timer_group_t;
typedef int timer_idx_t;
typedef int timer_count_dir_t;
typedef int timer_alarm_t;
typedef int timer_autoreload_t;
typedef bool timer_start_t;
typedef void (*timer_isr_t)(void*);

#define TIMER_GROUP_0 0
#define TIMER_GROUP_1 1
#define TIMER_0 0
#define TIMER_1 1
#define TIMER_COUNT_UP 0
#define TIMER_COUNT_DOWN 1
#define TIMER_ALARM_DIS 0
#define TIMER_ALARM_EN 1
#define TIMER_AUTORELOAD_DIS 0
#define TIMER_AUTORELOAD_EN 1
#define TIMER_PAUSE 0
#define TIMER_START 1

struct timer_config_t {
    timer_alarm_t alarm_en;
    timer_count_dir_t counter_dir;
    bool intr_type;  // simplified
    timer_autoreload_t auto_reload;
    uint32_t divider;
};

namespace esp32emu {
namespace timer_drv {

struct TimerState {
    timer_config_t config{};
    bool running = false;
    uint64_t counter = 0;
    uint64_t alarm_value = 0;
    bool alarm_enabled = false;
    std::chrono::steady_clock::time_point start_time;
    uint32_t divider = 80; // default: 1MHz with 80MHz APB
    timer_isr_t isr_cb = nullptr;
    void* isr_arg = nullptr;
};

using Key = std::pair<int, int>;

inline std::map<Key, TimerState>& timers() {
    static std::map<Key, TimerState> t;
    return t;
}

// Test helper: get timer state
inline TimerState& getTimer(int group, int idx) {
    return timers()[{group, idx}];
}

} // namespace timer_drv
} // namespace esp32emu

inline esp_err_t timer_init(timer_group_t group, timer_idx_t timer_num, const timer_config_t* config) {
    if (!config) return ESP_ERR_INVALID_ARG;
    auto& t = esp32emu::timer_drv::timers()[{group, timer_num}];
    t.config = *config;
    t.divider = config->divider;
    t.alarm_enabled = config->alarm_en == TIMER_ALARM_EN;
    fprintf(stderr, "[esp32emu] timer_init: group=%d timer=%d divider=%u dir=%d\n",
            group, timer_num, config->divider, config->counter_dir);
    return ESP_OK;
}

inline esp_err_t timer_set_counter_value(timer_group_t group, timer_idx_t timer_num, uint64_t load_val) {
    esp32emu::timer_drv::timers()[{group, timer_num}].counter = load_val;
    return ESP_OK;
}

inline esp_err_t timer_start(timer_group_t group, timer_idx_t timer_num) {
    auto& t = esp32emu::timer_drv::timers()[{group, timer_num}];
    t.running = true;
    t.start_time = std::chrono::steady_clock::now();
    fprintf(stderr, "[esp32emu] timer_start: group=%d timer=%d\n", group, timer_num);
    return ESP_OK;
}

inline esp_err_t timer_pause(timer_group_t group, timer_idx_t timer_num) {
    auto& t = esp32emu::timer_drv::timers()[{group, timer_num}];
    if (t.running) {
        auto elapsed = std::chrono::steady_clock::now() - t.start_time;
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        uint64_t ticks = (uint64_t)us * (80000000ULL / t.divider) / 1000000ULL;
        if (t.config.counter_dir == TIMER_COUNT_UP)
            t.counter += ticks;
        else if (t.counter > ticks)
            t.counter -= ticks;
        else
            t.counter = 0;
        t.running = false;
    }
    return ESP_OK;
}

inline esp_err_t timer_get_counter_value(timer_group_t group, timer_idx_t timer_num, uint64_t* timer_val) {
    auto& t = esp32emu::timer_drv::timers()[{group, timer_num}];
    uint64_t val = t.counter;
    if (t.running) {
        auto elapsed = std::chrono::steady_clock::now() - t.start_time;
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        uint64_t ticks = (uint64_t)us * (80000000ULL / t.divider) / 1000000ULL;
        if (t.config.counter_dir == TIMER_COUNT_UP)
            val += ticks;
        else if (val > ticks)
            val -= ticks;
        else
            val = 0;
    }
    *timer_val = val;
    return ESP_OK;
}

inline esp_err_t timer_set_alarm_value(timer_group_t group, timer_idx_t timer_num, uint64_t alarm_value) {
    esp32emu::timer_drv::timers()[{group, timer_num}].alarm_value = alarm_value;
    return ESP_OK;
}

inline esp_err_t timer_set_alarm(timer_group_t group, timer_idx_t timer_num, timer_alarm_t alarm_en) {
    esp32emu::timer_drv::timers()[{group, timer_num}].alarm_enabled = (alarm_en == TIMER_ALARM_EN);
    return ESP_OK;
}

inline esp_err_t timer_isr_register(timer_group_t group, timer_idx_t timer_num, timer_isr_t fn, void* arg, int intr_alloc_flags, void* handle) {
    (void)intr_alloc_flags; (void)handle;
    auto& t = esp32emu::timer_drv::timers()[{group, timer_num}];
    t.isr_cb = fn;
    t.isr_arg = arg;
    return ESP_OK;
}

inline esp_err_t timer_enable_intr(timer_group_t group, timer_idx_t timer_num) {
    (void)group; (void)timer_num;
    return ESP_OK;
}

inline esp_err_t timer_disable_intr(timer_group_t group, timer_idx_t timer_num) {
    (void)group; (void)timer_num;
    return ESP_OK;
}
