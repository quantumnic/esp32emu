#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <vector>
#include <map>
#include <mutex>
#include <functional>

// I2S modes
#define I2S_MODE_MASTER_TX  0x01
#define I2S_MODE_MASTER_RX  0x02
#define I2S_MODE_SLAVE_TX   0x04
#define I2S_MODE_SLAVE_RX   0x08
#define I2S_MODE_DAC_BUILT_IN 0x10
#define I2S_MODE_ADC_BUILT_IN 0x20
#define I2S_MODE_PDM        0x40

// I2S channel format
#define I2S_CHANNEL_FMT_RIGHT_LEFT   0x00
#define I2S_CHANNEL_FMT_ALL_RIGHT    0x01
#define I2S_CHANNEL_FMT_ALL_LEFT     0x02
#define I2S_CHANNEL_FMT_ONLY_RIGHT   0x03
#define I2S_CHANNEL_FMT_ONLY_LEFT    0x04

// I2S communication format
#define I2S_COMM_FORMAT_STAND_I2S    0x01
#define I2S_COMM_FORMAT_STAND_MSB    0x02
#define I2S_COMM_FORMAT_STAND_PCM_SHORT 0x04
#define I2S_COMM_FORMAT_STAND_PCM_LONG  0x08

// I2S port
#define I2S_NUM_0 0
#define I2S_NUM_1 1

namespace esp32emu {

struct I2SConfig {
    int mode = I2S_MODE_MASTER_TX;
    int sample_rate = 44100;
    int bits_per_sample = 16;
    int channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    int comm_format = I2S_COMM_FORMAT_STAND_I2S;
    int dma_buf_count = 8;
    int dma_buf_len = 64;
    int intr_alloc_flags = 0;
};

struct I2SPinConfig {
    int bck_io = -1;
    int ws_io = -1;
    int data_out = -1;
    int data_in = -1;
};

class I2SController {
public:
    static I2SController& instance() {
        static I2SController inst;
        return inst;
    }

    int install(int port, const I2SConfig& cfg) {
        std::lock_guard<std::mutex> lk(mtx_);
        ports_[port] = {cfg, {}, {}, {}, true};
        fprintf(stderr, "[esp32emu] I2S port %d installed: %dHz, %d-bit\n",
                port, cfg.sample_rate, cfg.bits_per_sample);
        return 0;
    }

    int setPins(int port, const I2SPinConfig& pins) {
        std::lock_guard<std::mutex> lk(mtx_);
        if (ports_.count(port)) ports_[port].pins = pins;
        return 0;
    }

    int start(int port) {
        std::lock_guard<std::mutex> lk(mtx_);
        if (ports_.count(port)) ports_[port].running = true;
        return 0;
    }

    int stop(int port) {
        std::lock_guard<std::mutex> lk(mtx_);
        if (ports_.count(port)) ports_[port].running = false;
        return 0;
    }

    int uninstall(int port) {
        std::lock_guard<std::mutex> lk(mtx_);
        ports_.erase(port);
        fprintf(stderr, "[esp32emu] I2S port %d uninstalled\n", port);
        return 0;
    }

    // Write audio data to I2S
    size_t write(int port, const void* data, size_t size, size_t* bytes_written, int timeout_ms) {
        (void)timeout_ms;
        std::lock_guard<std::mutex> lk(mtx_);
        if (!ports_.count(port) || !ports_[port].running) {
            if (bytes_written) *bytes_written = 0;
            return -1;
        }
        auto& buf = ports_[port].tx_buffer;
        const uint8_t* p = static_cast<const uint8_t*>(data);
        buf.insert(buf.end(), p, p + size);
        if (bytes_written) *bytes_written = size;
        return 0;
    }

    // Read audio data from I2S
    size_t read(int port, void* data, size_t size, size_t* bytes_read, int timeout_ms) {
        (void)timeout_ms;
        std::lock_guard<std::mutex> lk(mtx_);
        if (!ports_.count(port) || !ports_[port].running) {
            if (bytes_read) *bytes_read = 0;
            return -1;
        }
        auto& buf = ports_[port].rx_buffer;
        size_t avail = std::min(size, buf.size());
        if (avail > 0) {
            memcpy(data, buf.data(), avail);
            buf.erase(buf.begin(), buf.begin() + avail);
        }
        if (bytes_read) *bytes_read = avail;
        return 0;
    }

    // ── Test helpers ──

    // Inject RX data (simulates microphone input)
    void injectRxData(int port, const void* data, size_t size) {
        std::lock_guard<std::mutex> lk(mtx_);
        if (ports_.count(port)) {
            const uint8_t* p = static_cast<const uint8_t*>(data);
            ports_[port].rx_buffer.insert(ports_[port].rx_buffer.end(), p, p + size);
        }
    }

    // Get TX data (what was "played" to speaker)
    std::vector<uint8_t> getTxData(int port) {
        std::lock_guard<std::mutex> lk(mtx_);
        if (ports_.count(port)) return ports_[port].tx_buffer;
        return {};
    }

    // Get total bytes written
    size_t getTxSize(int port) {
        std::lock_guard<std::mutex> lk(mtx_);
        if (ports_.count(port)) return ports_[port].tx_buffer.size();
        return 0;
    }

    // Check if port is running
    bool isRunning(int port) {
        std::lock_guard<std::mutex> lk(mtx_);
        if (ports_.count(port)) return ports_[port].running;
        return false;
    }

    // Get config
    I2SConfig getConfig(int port) {
        std::lock_guard<std::mutex> lk(mtx_);
        if (ports_.count(port)) return ports_[port].config;
        return {};
    }

    void reset() {
        std::lock_guard<std::mutex> lk(mtx_);
        ports_.clear();
    }

private:
    struct PortState {
        I2SConfig config;
        I2SPinConfig pins;
        std::vector<uint8_t> tx_buffer;
        std::vector<uint8_t> rx_buffer;
        bool running = false;
    };

    std::mutex mtx_;
    std::map<int, PortState> ports_;
};

} // namespace esp32emu

// ── ESP-IDF compatible C API ──

typedef esp32emu::I2SConfig i2s_config_t;
typedef esp32emu::I2SPinConfig i2s_pin_config_t;
typedef int i2s_port_t;

inline int i2s_driver_install(i2s_port_t port, const i2s_config_t* cfg, int queue_size, void* queue) {
    (void)queue_size; (void)queue;
    return esp32emu::I2SController::instance().install(port, *cfg);
}

inline int i2s_set_pin(i2s_port_t port, const i2s_pin_config_t* pins) {
    return esp32emu::I2SController::instance().setPins(port, *pins);
}

inline int i2s_start(i2s_port_t port) {
    return esp32emu::I2SController::instance().start(port);
}

inline int i2s_stop(i2s_port_t port) {
    return esp32emu::I2SController::instance().stop(port);
}

inline int i2s_driver_uninstall(i2s_port_t port) {
    return esp32emu::I2SController::instance().uninstall(port);
}

inline int i2s_write(i2s_port_t port, const void* src, size_t size, size_t* bytes_written, int timeout_ms) {
    return esp32emu::I2SController::instance().write(port, src, size, bytes_written, timeout_ms);
}

inline int i2s_read(i2s_port_t port, void* dest, size_t size, size_t* bytes_read, int timeout_ms) {
    return esp32emu::I2SController::instance().read(port, dest, size, bytes_read, timeout_ms);
}
