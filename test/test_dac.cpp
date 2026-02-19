// esp32emu — test_dac: DAC driver mock test
#include "driver/dac.h"
#include <cassert>
#include <cstdio>

int main() {
    // Enable channel
    assert(dac_output_enable(DAC_CHANNEL_1) == ESP_OK);
    assert(esp32emu_dac_test::isEnabled(DAC_CHANNEL_1));

    // Set voltage
    assert(dac_output_voltage(DAC_CHANNEL_1, 128) == ESP_OK);
    assert(esp32emu_dac_test::getVoltage(DAC_CHANNEL_1) == 128);

    assert(dac_output_voltage(DAC_CHANNEL_1, 255) == ESP_OK);
    assert(esp32emu_dac_test::getVoltage(DAC_CHANNEL_1) == 255);

    // Disabled channel fails
    assert(!esp32emu_dac_test::isEnabled(DAC_CHANNEL_2));
    assert(dac_output_voltage(DAC_CHANNEL_2, 100) == ESP_FAIL);

    // Disable
    assert(dac_output_disable(DAC_CHANNEL_1) == ESP_OK);
    assert(!esp32emu_dac_test::isEnabled(DAC_CHANNEL_1));
    assert(dac_output_voltage(DAC_CHANNEL_1, 50) == ESP_FAIL);

    printf("test_dac: all assertions passed\n");
    return 0;
}
