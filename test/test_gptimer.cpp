// esp32emu – GPTimer driver tests
#include "driver/gptimer.h"
#include <cassert>
#include <cstdio>

static int alarm_count = 0;
static uint64_t last_alarm_value = 0;

static bool alarm_cb(gptimer_handle_t /*timer*/, const gptimer_alarm_event_data_t *edata, void * /*ctx*/) {
    alarm_count++;
    last_alarm_value = edata->alarm_value;
    return false;
}

int main() {
    gptimer_handle_t timer = nullptr;

    // Create timer
    gptimer_config_t cfg{};
    cfg.clk_src = GPTIMER_CLK_SRC_DEFAULT;
    cfg.direction = GPTIMER_COUNT_UP;
    cfg.resolution_hz = 1000000;  // 1MHz
    assert(gptimer_new_timer(&cfg, &timer) == ESP_OK);
    assert(timer != nullptr);

    // Set alarm at count 1000 with auto-reload
    gptimer_alarm_config_t alarm_cfg{};
    alarm_cfg.alarm_count = 1000;
    alarm_cfg.reload_count = 0;
    alarm_cfg.flags.auto_reload_on_alarm = 1;
    assert(gptimer_set_alarm_action(timer, &alarm_cfg) == ESP_OK);

    // Register callback
    gptimer_event_callbacks_t cbs{};
    cbs.on_alarm = alarm_cb;
    assert(gptimer_register_event_callbacks(timer, &cbs, nullptr) == ESP_OK);

    // Enable + start
    assert(gptimer_enable(timer) == ESP_OK);
    assert(esp32emu::gptimer::is_enabled(timer));
    assert(gptimer_start(timer) == ESP_OK);
    assert(esp32emu::gptimer::is_running(timer));

    // Simulate ticks — not enough for alarm
    alarm_count = 0;
    esp32emu::gptimer::simulate_ticks(timer, 500);
    assert(alarm_count == 0);
    uint64_t val = 0;
    gptimer_get_raw_count(timer, &val);
    assert(val == 500);

    // More ticks — triggers alarm, auto-reloads to 0
    esp32emu::gptimer::simulate_ticks(timer, 600);
    assert(alarm_count == 1);
    assert(last_alarm_value == 1000);
    gptimer_get_raw_count(timer, &val);
    assert(val == 0);  // auto-reloaded

    // Stop + disable
    assert(gptimer_stop(timer) == ESP_OK);
    assert(!esp32emu::gptimer::is_running(timer));
    assert(gptimer_disable(timer) == ESP_OK);

    // Can't start when disabled
    assert(gptimer_start(timer) == ESP_ERR_INVALID_STATE);

    // Set raw count
    assert(gptimer_set_raw_count(timer, 42) == ESP_OK);
    gptimer_get_raw_count(timer, &val);
    assert(val == 42);

    // Count down direction
    gptimer_handle_t timer2 = nullptr;
    gptimer_config_t cfg2{};
    cfg2.direction = GPTIMER_COUNT_DOWN;
    cfg2.resolution_hz = 1000;
    assert(gptimer_new_timer(&cfg2, &timer2) == ESP_OK);
    gptimer_set_raw_count(timer2, 500);
    alarm_cfg.alarm_count = 100;
    alarm_cfg.flags.auto_reload_on_alarm = 0;
    gptimer_set_alarm_action(timer2, &alarm_cfg);
    alarm_count = 0;
    cbs.on_alarm = alarm_cb;
    gptimer_register_event_callbacks(timer2, &cbs, nullptr);
    gptimer_enable(timer2);
    gptimer_start(timer2);
    esp32emu::gptimer::simulate_ticks(timer2, 450);
    assert(alarm_count == 1);

    // Cleanup
    gptimer_disable(timer2);
    gptimer_del_timer(timer2);
    gptimer_del_timer(timer);

    printf("test_gptimer: all assertions passed\n");
    return 0;
}
