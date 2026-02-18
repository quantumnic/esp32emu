/*
 * I2S Tone Generator — ESP32 I2S audio output demo
 *
 * Generates a 440Hz sine wave and outputs it via I2S.
 * Demonstrates I2S driver setup, pin configuration, and audio streaming.
 *
 * Board: ESP32
 * Pins: BCK=26, WS=25, DATA=22
 */

#include <Arduino.h>
#include "I2S.h"
#include <cmath>

#define SAMPLE_RATE  44100
#define FREQUENCY    440
#define AMPLITUDE    16000
#define DURATION_MS  2000

void setup() {
    Serial.begin(115200);
    Serial.println("🔊 I2S Tone Generator — 440Hz sine wave");

    // Configure I2S
    i2s_config_t cfg = {};
    cfg.mode = I2S_MODE_MASTER_TX;
    cfg.sample_rate = SAMPLE_RATE;
    cfg.bits_per_sample = 16;
    cfg.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    cfg.comm_format = I2S_COMM_FORMAT_STAND_I2S;
    cfg.dma_buf_count = 8;
    cfg.dma_buf_len = 64;

    i2s_driver_install(I2S_NUM_0, &cfg, 0, nullptr);

    i2s_pin_config_t pins = {26, 25, 22, -1};
    i2s_set_pin(I2S_NUM_0, &pins);
    i2s_start(I2S_NUM_0);

    Serial.printf("Sample rate: %d Hz, Duration: %d ms\n", SAMPLE_RATE, DURATION_MS);

    // Generate and stream sine wave
    int total_samples = (SAMPLE_RATE * DURATION_MS) / 1000;
    int chunk_size = 256;
    int16_t buffer[512];  // stereo: 256 samples * 2 channels

    int samples_sent = 0;
    while (samples_sent < total_samples) {
        int count = std::min(chunk_size, total_samples - samples_sent);
        for (int i = 0; i < count; i++) {
            double t = (double)(samples_sent + i) / SAMPLE_RATE;
            int16_t sample = (int16_t)(AMPLITUDE * sin(2.0 * M_PI * FREQUENCY * t));
            buffer[i * 2] = sample;      // left channel
            buffer[i * 2 + 1] = sample;  // right channel
        }

        size_t written = 0;
        i2s_write(I2S_NUM_0, buffer, count * 4, &written, 100);
        samples_sent += count;
    }

    Serial.printf("✅ Streamed %d samples (%.1f seconds)\n",
                  total_samples, (float)DURATION_MS / 1000.0);

    i2s_stop(I2S_NUM_0);
    i2s_driver_uninstall(I2S_NUM_0);
    Serial.println("I2S driver stopped and uninstalled.");
}

void loop() {
    delay(1000);
}
