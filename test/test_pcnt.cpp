#include "driver/pcnt.h"
#include <cassert>
#include <cstdio>

int main() {
    // Configure unit 0 for counting up
    pcnt_config_t config{};
    config.unit = PCNT_UNIT_0;
    config.channel = PCNT_CHANNEL_0;
    config.pulse_gpio_num = 4;
    config.ctrl_gpio_num = 5;
    config.pos_mode = PCNT_COUNT_INC;
    config.neg_mode = PCNT_COUNT_DIS;
    config.counter_h_lim = 100;
    config.counter_l_lim = -100;
    assert(pcnt_unit_config(&config) == ESP_OK);

    // Counter starts at zero
    int16_t count = 0;
    assert(pcnt_get_counter_value(PCNT_UNIT_0, &count) == ESP_OK);
    assert(count == 0);

    // Inject pulses and read
    esp32emu::pcnt::injectPulses(PCNT_UNIT_0, 10);
    assert(pcnt_get_counter_value(PCNT_UNIT_0, &count) == ESP_OK);
    assert(count == 10);

    // Clear counter
    assert(pcnt_counter_clear(PCNT_UNIT_0) == ESP_OK);
    assert(pcnt_get_counter_value(PCNT_UNIT_0, &count) == ESP_OK);
    assert(count == 0);

    // Pause/resume (should not crash)
    assert(pcnt_counter_pause(PCNT_UNIT_0) == ESP_OK);
    assert(pcnt_counter_resume(PCNT_UNIT_0) == ESP_OK);

    // Filter
    assert(pcnt_set_filter_value(PCNT_UNIT_0, 100) == ESP_OK);
    assert(pcnt_filter_enable(PCNT_UNIT_0) == ESP_OK);
    assert(esp32emu::pcnt::getUnit(PCNT_UNIT_0).filter_enabled == true);
    assert(esp32emu::pcnt::getUnit(PCNT_UNIT_0).filter_value == 100);
    assert(pcnt_filter_disable(PCNT_UNIT_0) == ESP_OK);

    // High limit wrapping: inject enough to exceed h_lim
    esp32emu::pcnt::injectPulses(PCNT_UNIT_0, 150);
    assert(pcnt_get_counter_value(PCNT_UNIT_0, &count) == ESP_OK);
    assert(count < 100); // wrapped at h_lim

    // Events (just verify they don't crash)
    assert(pcnt_set_event_value(PCNT_UNIT_0, PCNT_EVT_THRES_0, 50) == ESP_OK);
    assert(pcnt_event_enable(PCNT_UNIT_0, PCNT_EVT_THRES_0) == ESP_OK);
    assert(pcnt_event_disable(PCNT_UNIT_0, PCNT_EVT_THRES_0) == ESP_OK);

    // Count down mode
    pcnt_config_t config2{};
    config2.unit = PCNT_UNIT_1;
    config2.channel = PCNT_CHANNEL_0;
    config2.pulse_gpio_num = 6;
    config2.ctrl_gpio_num = 7;
    config2.pos_mode = PCNT_COUNT_DEC;
    config2.neg_mode = PCNT_COUNT_DIS;
    config2.counter_h_lim = 0;
    config2.counter_l_lim = -100;
    assert(pcnt_unit_config(&config2) == ESP_OK);

    esp32emu::pcnt::injectPulses(PCNT_UNIT_1, 5);
    assert(pcnt_get_counter_value(PCNT_UNIT_1, &count) == ESP_OK);
    assert(count == -5);

    printf("test_pcnt: all assertions passed\n");
    return 0;
}
