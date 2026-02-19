#include "driver/timer.h"
#include <cassert>
#include <cstdio>
#include <thread>
#include <chrono>

int main() {
    // Init timer
    timer_config_t config{};
    config.divider = 80;  // 80MHz / 80 = 1MHz (1us per tick)
    config.counter_dir = TIMER_COUNT_UP;
    config.alarm_en = TIMER_ALARM_DIS;
    config.auto_reload = TIMER_AUTORELOAD_DIS;

    assert(timer_init(TIMER_GROUP_0, TIMER_0, &config) == ESP_OK);
    assert(timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0) == ESP_OK);

    // Start and measure
    assert(timer_start(TIMER_GROUP_0, TIMER_0) == ESP_OK);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    uint64_t val = 0;
    assert(timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &val) == ESP_OK);
    // Should be roughly 10000 ticks (10ms at 1MHz), allow wide margin
    assert(val > 5000);
    assert(val < 50000);

    // Pause and verify counter stops
    assert(timer_pause(TIMER_GROUP_0, TIMER_0) == ESP_OK);
    uint64_t val_paused = 0;
    timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &val_paused);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    uint64_t val_after = 0;
    timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &val_after);
    assert(val_paused == val_after); // shouldn't change while paused

    // Alarm value
    assert(timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 100000) == ESP_OK);
    assert(timer_set_alarm(TIMER_GROUP_0, TIMER_0, TIMER_ALARM_EN) == ESP_OK);
    auto& t = esp32emu::timer_drv::getTimer(TIMER_GROUP_0, TIMER_0);
    assert(t.alarm_enabled == true);
    assert(t.alarm_value == 100000);

    // Count down timer
    timer_config_t config2{};
    config2.divider = 80;
    config2.counter_dir = TIMER_COUNT_DOWN;
    config2.alarm_en = TIMER_ALARM_DIS;
    config2.auto_reload = TIMER_AUTORELOAD_DIS;

    assert(timer_init(TIMER_GROUP_1, TIMER_0, &config2) == ESP_OK);
    assert(timer_set_counter_value(TIMER_GROUP_1, TIMER_0, 100000) == ESP_OK);
    assert(timer_start(TIMER_GROUP_1, TIMER_0) == ESP_OK);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    uint64_t down_val = 0;
    timer_get_counter_value(TIMER_GROUP_1, TIMER_0, &down_val);
    assert(down_val < 100000); // should have counted down
    timer_pause(TIMER_GROUP_1, TIMER_0);

    // ISR register (just verifying it doesn't crash)
    assert(timer_isr_register(TIMER_GROUP_0, TIMER_0, nullptr, nullptr, 0, nullptr) == ESP_OK);
    assert(timer_enable_intr(TIMER_GROUP_0, TIMER_0) == ESP_OK);
    assert(timer_disable_intr(TIMER_GROUP_0, TIMER_0) == ESP_OK);

    printf("test_timer_driver: all assertions passed\n");
    return 0;
}
