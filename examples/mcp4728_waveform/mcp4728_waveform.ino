// MCP4728 4-channel DAC waveform generator
#include <Wire.h>
#include <Adafruit_MCP4728.h>
#include <math.h>

Adafruit_MCP4728 dac;
uint16_t step = 0;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    if (!dac.begin()) {
        Serial.println("MCP4728 not found!");
        while (1) delay(1000);
    }

    // Channel A: sine, B: sawtooth, C: triangle, D: square
    dac.setChannelValue(MCP4728_CHANNEL_A, 0, MCP4728_VREF_INTERNAL, MCP4728_GAIN_2X);
    Serial.println("MCP4728 waveform generator ready");
}

void loop() {
    float phase = (step % 360) * M_PI / 180.0;

    uint16_t sine     = (uint16_t)(2047.5 + 2047.5 * sin(phase));
    uint16_t sawtooth = (uint16_t)((step % 4096));
    uint16_t triangle = (step % 8192 < 4096) ? (step % 4096) : (4095 - (step % 4096));
    uint16_t square   = (step % 360 < 180) ? 4095 : 0;

    dac.fastWrite(sine, sawtooth, triangle, square);

    if (step % 90 == 0) {
        Serial.print("Step "); Serial.print(step);
        Serial.print(" — Sin="); Serial.print(sine);
        Serial.print(" Saw="); Serial.print(sawtooth);
        Serial.print(" Tri="); Serial.print(triangle);
        Serial.print(" Sq="); Serial.println(square);
    }

    step++;
    delay(1);
}
