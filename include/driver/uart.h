// esp32emu – ESP-IDF UART driver mock
#pragma once
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <vector>
#include <mutex>
#include <algorithm>

#ifndef ESP_OK
#define ESP_OK   0
#define ESP_FAIL -1
#define ESP_ERR_INVALID_ARG -2
#endif
typedef int esp_err_t;

typedef int uart_port_t;
typedef int uart_word_length_t;
typedef int uart_stop_bits_t;
typedef int uart_parity_t;
typedef int uart_hw_flowcontrol_t;
typedef int uart_mode_t;

#define UART_NUM_0  0
#define UART_NUM_1  1
#define UART_NUM_2  2

#define UART_DATA_5_BITS  0
#define UART_DATA_6_BITS  1
#define UART_DATA_7_BITS  2
#define UART_DATA_8_BITS  3

#define UART_STOP_BITS_1    0
#define UART_STOP_BITS_1_5  1
#define UART_STOP_BITS_2    2

#define UART_PARITY_DISABLE 0
#define UART_PARITY_EVEN    1
#define UART_PARITY_ODD     2

#define UART_HW_FLOWCTRL_DISABLE  0
#define UART_HW_FLOWCTRL_RTS      1
#define UART_HW_FLOWCTRL_CTS      2
#define UART_HW_FLOWCTRL_CTS_RTS  3

#define UART_MODE_UART  0

#define UART_PIN_NO_CHANGE -1

struct uart_config_t {
    int baud_rate;
    uart_word_length_t data_bits;
    uart_parity_t parity;
    uart_stop_bits_t stop_bits;
    uart_hw_flowcontrol_t flow_ctrl;
    int rx_flow_ctrl_thresh;
    int source_clk;
};

namespace esp32emu {

class UartPort {
public:
    void configure(const uart_config_t& cfg) { config_ = cfg; }

    void injectRx(const uint8_t* data, size_t len) {
        std::lock_guard<std::mutex> lk(mu_);
        rxBuf_.insert(rxBuf_.end(), data, data + len);
    }

    int read(uint8_t* buf, size_t len, int ticksToWait) {
        (void)ticksToWait;
        std::lock_guard<std::mutex> lk(mu_);
        size_t n = std::min(len, rxBuf_.size());
        if (n > 0) {
            std::memcpy(buf, rxBuf_.data(), n);
            rxBuf_.erase(rxBuf_.begin(), rxBuf_.begin() + (int)n);
        }
        return (int)n;
    }

    int write(const uint8_t* data, size_t len) {
        std::lock_guard<std::mutex> lk(mu_);
        txBuf_.insert(txBuf_.end(), data, data + len);
        return (int)len;
    }

    std::vector<uint8_t> getTxData() {
        std::lock_guard<std::mutex> lk(mu_);
        auto d = txBuf_;
        txBuf_.clear();
        return d;
    }

    size_t rxAvailable() {
        std::lock_guard<std::mutex> lk(mu_);
        return rxBuf_.size();
    }

    void flush() {
        std::lock_guard<std::mutex> lk(mu_);
        txBuf_.clear();
    }

    void reset() {
        std::lock_guard<std::mutex> lk(mu_);
        rxBuf_.clear();
        txBuf_.clear();
    }

private:
    std::mutex mu_;
    uart_config_t config_{};
    std::vector<uint8_t> rxBuf_;
    std::vector<uint8_t> txBuf_;
};

class UartDriver {
public:
    static UartDriver& instance() {
        static UartDriver inst;
        return inst;
    }

    UartPort& port(uart_port_t num) {
        if (num < 0 || num > 2) num = 0;
        return ports_[num];
    }

private:
    UartPort ports_[3];
};

} // namespace esp32emu

inline esp_err_t uart_param_config(uart_port_t port, const uart_config_t* cfg) {
    if (!cfg) return ESP_ERR_INVALID_ARG;
    esp32emu::UartDriver::instance().port(port).configure(*cfg);
    return ESP_OK;
}

inline esp_err_t uart_set_pin(uart_port_t port, int tx, int rx, int rts, int cts) {
    (void)port; (void)tx; (void)rx; (void)rts; (void)cts;
    return ESP_OK;
}

inline esp_err_t uart_driver_install(uart_port_t port, int rxBufSize, int txBufSize, int queueSize, void* queue, int flags) {
    (void)port; (void)rxBufSize; (void)txBufSize; (void)queueSize; (void)queue; (void)flags;
    return ESP_OK;
}

inline esp_err_t uart_driver_delete(uart_port_t port) {
    esp32emu::UartDriver::instance().port(port).reset();
    return ESP_OK;
}

inline int uart_read_bytes(uart_port_t port, uint8_t* buf, size_t len, int ticksToWait) {
    return esp32emu::UartDriver::instance().port(port).read(buf, len, ticksToWait);
}

inline int uart_write_bytes(uart_port_t port, const void* data, size_t len) {
    return esp32emu::UartDriver::instance().port(port).write((const uint8_t*)data, len);
}

inline esp_err_t uart_flush(uart_port_t port) {
    esp32emu::UartDriver::instance().port(port).flush();
    return ESP_OK;
}

inline esp_err_t uart_get_buffered_data_len(uart_port_t port, size_t* size) {
    if (!size) return ESP_ERR_INVALID_ARG;
    *size = esp32emu::UartDriver::instance().port(port).rxAvailable();
    return ESP_OK;
}
