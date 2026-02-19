// Rotary encoder example using PCNT (Pulse Counter)
#include "driver/pcnt.h"
#include <cstdio>

#define ENCODER_A_PIN 34
#define ENCODER_B_PIN 35
#define PCNT_H_LIM   1000
#define PCNT_L_LIM  -1000

void setup() {
    pcnt_config_t config{};
    config.pulse_gpio_num = ENCODER_A_PIN;
    config.ctrl_gpio_num = ENCODER_B_PIN;
    config.unit = PCNT_UNIT_0;
    config.channel = PCNT_CHANNEL_0;
    config.pos_mode = PCNT_COUNT_INC;
    config.neg_mode = PCNT_COUNT_DEC;
    config.lctrl_mode = PCNT_MODE_REVERSE;
    config.hctrl_mode = PCNT_MODE_KEEP;
    config.counter_h_lim = PCNT_H_LIM;
    config.counter_l_lim = PCNT_L_LIM;

    pcnt_unit_config(&config);
    pcnt_set_filter_value(PCNT_UNIT_0, 100);
    pcnt_filter_enable(PCNT_UNIT_0);
    pcnt_counter_clear(PCNT_UNIT_0);

    printf("Rotary encoder initialized on pins %d/%d\n", ENCODER_A_PIN, ENCODER_B_PIN);

    // Simulate rotation for demo
    esp32emu::pcnt::injectPulses(PCNT_UNIT_0, 42);

    int16_t count = 0;
    pcnt_get_counter_value(PCNT_UNIT_0, &count);
    printf("Encoder position: %d\n", count);
}

int main() { setup(); return 0; }
