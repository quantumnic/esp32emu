#pragma once
#include <cstdint>
#include <cstring>
#include <vector>
#include <functional>

#ifdef __cplusplus
extern "C" {
#endif

// ESP-IDF SPI slave driver mock

typedef int esp_err_t;
#ifndef ESP_OK
#define ESP_OK 0
#endif
#ifndef ESP_ERR_INVALID_ARG
#define ESP_ERR_INVALID_ARG 0x102
#endif

typedef int spi_host_device_t;
#ifndef SPI2_HOST
#define SPI2_HOST 1
#endif
#ifndef SPI3_HOST
#define SPI3_HOST 2
#endif

typedef struct {
    int spics_io_num;
    uint32_t flags;
    int queue_size;
    uint8_t mode;
    void (*post_setup_cb)(void*);
    void (*post_trans_cb)(void*);
} spi_slave_interface_config_t;

typedef struct {
    size_t length;     // Total data length, in bits
    size_t trans_len;  // Actual transferred length, in bits
    void *tx_buffer;
    void *rx_buffer;
    void *user;
} spi_slave_transaction_t;

typedef struct {
    int mosi_io_num;
    int miso_io_num;
    int sclk_io_num;
    int quadwp_io_num;
    int quadhd_io_num;
} spi_bus_config_t;

namespace esp32emu_spi_slave {
    struct State {
        bool initialized = false;
        spi_host_device_t host = SPI2_HOST;
        std::vector<spi_slave_transaction_t*> queued;
        std::vector<uint8_t> inject_rx;  // test: data slave will "receive" from master

        static State& instance() { static State s; return s; }
        void reset() { initialized = false; queued.clear(); inject_rx.clear(); }
    };
}

inline esp_err_t spi_slave_initialize(spi_host_device_t host,
                                       const spi_bus_config_t* /*bus_config*/,
                                       const spi_slave_interface_config_t* /*slave_config*/,
                                       int /*dma_chan*/) {
    auto& s = esp32emu_spi_slave::State::instance();
    s.host = host;
    s.initialized = true;
    return ESP_OK;
}

inline esp_err_t spi_slave_free(spi_host_device_t /*host*/) {
    auto& s = esp32emu_spi_slave::State::instance();
    s.reset();
    return ESP_OK;
}

inline esp_err_t spi_slave_transmit(spi_host_device_t /*host*/,
                                     spi_slave_transaction_t* trans,
                                     uint32_t /*ticks_to_wait*/) {
    auto& s = esp32emu_spi_slave::State::instance();
    if (!s.initialized) return ESP_ERR_INVALID_ARG;

    size_t bytes = trans->length / 8;
    // Fill rx_buffer with injected data if available
    if (trans->rx_buffer && !s.inject_rx.empty()) {
        size_t copy = bytes < s.inject_rx.size() ? bytes : s.inject_rx.size();
        memcpy(trans->rx_buffer, s.inject_rx.data(), copy);
        s.inject_rx.erase(s.inject_rx.begin(), s.inject_rx.begin() + copy);
    }
    trans->trans_len = trans->length;
    s.queued.push_back(trans);
    return ESP_OK;
}

inline esp_err_t spi_slave_queue_trans(spi_host_device_t /*host*/,
                                        const spi_slave_transaction_t* trans,
                                        uint32_t /*ticks_to_wait*/) {
    auto& s = esp32emu_spi_slave::State::instance();
    if (!s.initialized) return ESP_ERR_INVALID_ARG;
    s.queued.push_back(const_cast<spi_slave_transaction_t*>(trans));
    return ESP_OK;
}

// Test helpers
namespace esp32emu_spi_slave {
    inline void injectRxData(const uint8_t* data, size_t len) {
        auto& s = State::instance();
        s.inject_rx.insert(s.inject_rx.end(), data, data + len);
    }
    inline void reset() { State::instance().reset(); }
    inline size_t getTransactionCount() { return State::instance().queued.size(); }
}

#ifdef __cplusplus
}
#endif
