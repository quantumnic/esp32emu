#pragma once
// esp32emu – ESP-IDF v5 GPTimer driver mock
// General Purpose Timer with alarm, auto-reload, start/stop.

#include <cstdint>
#include <functional>
#include <mutex>
#include <vector>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

// ── Types ───────────────────────────────────────────────────────────
typedef enum {
    GPTIMER_CLK_SRC_DEFAULT = 0,
    GPTIMER_CLK_SRC_APB,
    GPTIMER_CLK_SRC_XTAL,
} gptimer_clock_source_t;

typedef enum {
    GPTIMER_COUNT_DOWN = 0,
    GPTIMER_COUNT_UP,
} gptimer_count_direction_t;

typedef struct gptimer_t *gptimer_handle_t;

typedef struct {
    gptimer_clock_source_t clk_src;
    gptimer_count_direction_t direction;
    uint32_t resolution_hz;
    int intr_priority;
    struct {
        uint32_t intr_shared : 1;
    } flags;
} gptimer_config_t;

typedef struct {
    uint64_t alarm_count;
    uint64_t reload_count;
    struct {
        uint32_t auto_reload_on_alarm : 1;
    } flags;
} gptimer_alarm_config_t;

// Alarm event data
typedef struct {
    uint64_t count_value;
    uint64_t alarm_value;
} gptimer_alarm_event_data_t;

// Alarm callback
typedef bool (*gptimer_alarm_cb_t)(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx);

typedef struct {
    gptimer_alarm_cb_t on_alarm;
} gptimer_event_callbacks_t;

// ── Internal state ──────────────────────────────────────────────────
struct gptimer_t {
    gptimer_config_t config{};
    gptimer_alarm_config_t alarm_config{};
    bool alarm_enabled = false;
    bool running = false;
    bool enabled = false;
    uint64_t count = 0;
    gptimer_alarm_cb_t alarm_cb = nullptr;
    void *user_ctx = nullptr;
    std::mutex mtx;
};

// ── API ─────────────────────────────────────────────────────────────
inline esp_err_t gptimer_new_timer(const gptimer_config_t *config, gptimer_handle_t *ret_timer) {
    if (!config || !ret_timer) return ESP_ERR_INVALID_ARG;
    auto *t = new gptimer_t();
    t->config = *config;
    *ret_timer = t;
    return ESP_OK;
}

inline esp_err_t gptimer_del_timer(gptimer_handle_t timer) {
    if (!timer) return ESP_ERR_INVALID_ARG;
    delete timer;
    return ESP_OK;
}

inline esp_err_t gptimer_set_alarm_action(gptimer_handle_t timer, const gptimer_alarm_config_t *config) {
    if (!timer) return ESP_ERR_INVALID_ARG;
    std::lock_guard<std::mutex> lk(timer->mtx);
    if (config) {
        timer->alarm_config = *config;
        timer->alarm_enabled = true;
    } else {
        timer->alarm_enabled = false;
    }
    return ESP_OK;
}

inline esp_err_t gptimer_register_event_callbacks(gptimer_handle_t timer, const gptimer_event_callbacks_t *cbs, void *user_ctx) {
    if (!timer || !cbs) return ESP_ERR_INVALID_ARG;
    std::lock_guard<std::mutex> lk(timer->mtx);
    timer->alarm_cb = cbs->on_alarm;
    timer->user_ctx = user_ctx;
    return ESP_OK;
}

inline esp_err_t gptimer_enable(gptimer_handle_t timer) {
    if (!timer) return ESP_ERR_INVALID_ARG;
    timer->enabled = true;
    return ESP_OK;
}

inline esp_err_t gptimer_disable(gptimer_handle_t timer) {
    if (!timer) return ESP_ERR_INVALID_ARG;
    timer->enabled = false;
    timer->running = false;
    return ESP_OK;
}

inline esp_err_t gptimer_start(gptimer_handle_t timer) {
    if (!timer) return ESP_ERR_INVALID_ARG;
    if (!timer->enabled) return ESP_ERR_INVALID_STATE;
    timer->running = true;
    return ESP_OK;
}

inline esp_err_t gptimer_stop(gptimer_handle_t timer) {
    if (!timer) return ESP_ERR_INVALID_ARG;
    timer->running = false;
    return ESP_OK;
}

inline esp_err_t gptimer_set_raw_count(gptimer_handle_t timer, uint64_t value) {
    if (!timer) return ESP_ERR_INVALID_ARG;
    std::lock_guard<std::mutex> lk(timer->mtx);
    timer->count = value;
    return ESP_OK;
}

inline esp_err_t gptimer_get_raw_count(gptimer_handle_t timer, uint64_t *value) {
    if (!timer || !value) return ESP_ERR_INVALID_ARG;
    std::lock_guard<std::mutex> lk(timer->mtx);
    *value = timer->count;
    return ESP_OK;
}

#ifdef __cplusplus
}

// ── Test helpers ────────────────────────────────────────────────────
namespace esp32emu {
namespace gptimer {

// Simulate timer ticks — advances count and fires alarm if threshold reached
inline void simulate_ticks(gptimer_handle_t timer, uint64_t ticks) {
    if (!timer || !timer->running) return;
    std::lock_guard<std::mutex> lk(timer->mtx);
    
    if (timer->config.direction == GPTIMER_COUNT_UP) {
        timer->count += ticks;
        if (timer->alarm_enabled && timer->count >= timer->alarm_config.alarm_count) {
            if (timer->alarm_cb) {
                gptimer_alarm_event_data_t edata{};
                edata.count_value = timer->count;
                edata.alarm_value = timer->alarm_config.alarm_count;
                timer->alarm_cb(timer, &edata, timer->user_ctx);
            }
            if (timer->alarm_config.flags.auto_reload_on_alarm) {
                timer->count = timer->alarm_config.reload_count;
            }
        }
    } else {
        if (ticks > timer->count) timer->count = 0;
        else timer->count -= ticks;
        if (timer->alarm_enabled && timer->count <= timer->alarm_config.alarm_count) {
            if (timer->alarm_cb) {
                gptimer_alarm_event_data_t edata{};
                edata.count_value = timer->count;
                edata.alarm_value = timer->alarm_config.alarm_count;
                timer->alarm_cb(timer, &edata, timer->user_ctx);
            }
            if (timer->alarm_config.flags.auto_reload_on_alarm) {
                timer->count = timer->alarm_config.reload_count;
            }
        }
    }
}

// Get the current state
inline bool is_running(gptimer_handle_t timer) { return timer && timer->running; }
inline bool is_enabled(gptimer_handle_t timer) { return timer && timer->enabled; }

} // namespace gptimer
} // namespace esp32emu
#endif
