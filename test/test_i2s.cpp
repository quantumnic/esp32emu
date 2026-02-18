#include "I2S.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    auto& i2s = esp32emu::I2SController::instance();
    i2s.reset();

    // Install I2S driver
    i2s_config_t cfg = {};
    cfg.mode = I2S_MODE_MASTER_TX;
    cfg.sample_rate = 44100;
    cfg.bits_per_sample = 16;
    cfg.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;

    int ret = i2s_driver_install(I2S_NUM_0, &cfg, 0, nullptr);
    assert(ret == 0);

    // Set pins
    i2s_pin_config_t pins = {26, 25, 22, -1};
    ret = i2s_set_pin(I2S_NUM_0, &pins);
    assert(ret == 0);

    // Start
    ret = i2s_start(I2S_NUM_0);
    assert(ret == 0);
    assert(i2s.isRunning(I2S_NUM_0));

    // Write audio data
    int16_t samples[4] = {1000, -1000, 2000, -2000};
    size_t written = 0;
    ret = i2s_write(I2S_NUM_0, samples, sizeof(samples), &written, 100);
    assert(ret == 0);
    assert(written == sizeof(samples));
    assert(i2s.getTxSize(I2S_NUM_0) == sizeof(samples));

    // Verify TX data
    auto tx = i2s.getTxData(I2S_NUM_0);
    assert(tx.size() == sizeof(samples));
    assert(memcmp(tx.data(), samples, sizeof(samples)) == 0);

    // Test RX (inject microphone data)
    i2s_config_t rx_cfg = {};
    rx_cfg.mode = I2S_MODE_MASTER_RX;
    rx_cfg.sample_rate = 16000;
    rx_cfg.bits_per_sample = 16;
    i2s_driver_install(I2S_NUM_1, &rx_cfg, 0, nullptr);
    i2s_start(I2S_NUM_1);

    int16_t mic_data[2] = {500, -500};
    i2s.injectRxData(I2S_NUM_1, mic_data, sizeof(mic_data));

    int16_t read_buf[2] = {};
    size_t bytes_read = 0;
    ret = i2s_read(I2S_NUM_1, read_buf, sizeof(read_buf), &bytes_read, 100);
    assert(ret == 0);
    assert(bytes_read == sizeof(mic_data));
    assert(read_buf[0] == 500);
    assert(read_buf[1] == -500);

    // Stop and uninstall
    ret = i2s_stop(I2S_NUM_0);
    assert(ret == 0);
    assert(!i2s.isRunning(I2S_NUM_0));

    ret = i2s_driver_uninstall(I2S_NUM_0);
    assert(ret == 0);

    // Verify config retrieval
    auto rcfg = i2s.getConfig(I2S_NUM_1);
    assert(rcfg.sample_rate == 16000);

    i2s_driver_uninstall(I2S_NUM_1);

    printf("test_i2s: all assertions passed\n");
    return 0;
}
