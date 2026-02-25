// DAC continuous sine wave generator
// Outputs a sine wave using ESP-IDF DAC DMA driver

#include <Arduino.h>
#include <driver/dac_continuous.h>
#include <cmath>

#define SAMPLE_RATE   8000
#define WAVE_FREQ     440   // A4 note
#define BUF_SIZE      256

static uint8_t wave_buf[BUF_SIZE];

void setup() {
    Serial.begin(115200);
    Serial.println("DAC Continuous Sine Wave Generator");

    // Generate sine wave buffer
    for (int i = 0; i < BUF_SIZE; i++) {
        wave_buf[i] = (uint8_t)(128 + 127 * sin(2.0 * M_PI * WAVE_FREQ * i / SAMPLE_RATE));
    }
    Serial.print("Generated ");
    Serial.print(BUF_SIZE);
    Serial.println(" samples");

    // Configure DAC continuous mode
    dac_continuous_config_t cfg = {};
    cfg.chan_mask = DAC_CHAN_0;
    cfg.desc_num = 8;
    cfg.buf_size = BUF_SIZE;
    cfg.freq_hz = SAMPLE_RATE;

    dac_continuous_handle_t handle;
    if (dac_continuous_new_channels(&cfg, &handle) != 0) {
        Serial.println("ERROR: Failed to init DAC!");
        return;
    }

    dac_continuous_enable(handle);

    // Write cyclic waveform
    size_t loaded = 0;
    dac_continuous_write_cyclically(handle, wave_buf, BUF_SIZE, &loaded);

    Serial.print("Playing ");
    Serial.print(WAVE_FREQ);
    Serial.print("Hz tone at ");
    Serial.print(SAMPLE_RATE);
    Serial.println("Hz sample rate");
    Serial.print("Loaded ");
    Serial.print(loaded);
    Serial.println(" bytes to DAC");
}

void loop() {
    delay(1000);
    Serial.println("♪ Sine wave playing...");
}
