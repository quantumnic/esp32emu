// esp32emu example — MCP4725 DAC: generate a sawtooth wave
#include <Arduino.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;

void setup() {
    Serial.begin(115200);
    if (!dac.begin()) {
        Serial.println("MCP4725 not found!");
        while (1) delay(10);
    }
    Serial.println("MCP4725 DAC ready");
}

void loop() {
    // Sawtooth wave: ramp from 0 to 4095
    for (uint16_t val = 0; val < 4096; val += 64) {
        dac.setVoltage(val, false);
        float voltage = (val / 4095.0f) * 3.3f;
        Serial.printf("DAC: %u (%.2f V)\n", val, voltage);
        delay(10);
    }
}
