// Example: DAC waveform — outputs a sawtooth wave on DAC channel 1
#include <Arduino.h>
#include "driver/dac.h"
#include <cstdio>

void setup() {
    dac_output_enable(DAC_CHANNEL_1);
    printf("DAC channel 1 enabled — generating sawtooth wave\n");
}

void loop() {
    for (int i = 0; i < 256; i++) {
        dac_output_voltage(DAC_CHANNEL_1, static_cast<uint8_t>(i));
        delay(2);
    }
}
