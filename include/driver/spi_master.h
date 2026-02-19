// esp32emu – ESP-IDF SPI master driver mock
#pragma once
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <vector>
#include <functional>
#include <mutex>

#ifndef ESP_OK
#define ESP_OK   0
#define ESP_FAIL -1
#define ESP_ERR_INVALID_ARG -2
#endif
typedef int esp_err_t;

typedef int spi_host_device_t;

#define SPI1_HOST  0
#define SPI2_HOST  1
#define SPI3_HOST  2
#define HSPI_HOST  SPI2_HOST
#define VSPI_HOST  SPI3_HOST

typedef void* spi_device_handle_t;

struct spi_bus_config_t {
    int mosi_io_num;
    int miso_io_num;
    int sclk_io_num;
    int quadwp_io_num;
    int quadhd_io_num;
    int max_transfer_sz;
};

struct spi_device_interface_config_t {
    int command_bits;
    int address_bits;
    int dummy_bits;
    int mode;
    int duty_cycle_pos;
    int cs_ena_pretrans;
    int cs_ena_posttrans;
    int clock_speed_hz;
    int input_delay_ns;
    int spics_io_num;
    int flags;
    int queue_size;
    void (*pre_cb)(void*);
    void (*post_cb)(void*);
};

struct spi_transaction_t {
    uint32_t flags;
    uint16_t cmd;
    uint64_t addr;
    size_t length;     // in bits
    size_t rxlength;   // in bits
    void* user;
    const void* tx_buffer;
    void* rx_buffer;
    uint8_t tx_data[4];
    uint8_t rx_data[4];
};

#define SPI_TRANS_USE_TXDATA  (1 << 0)
#define SPI_TRANS_USE_RXDATA  (1 << 1)

namespace esp32emu {

// Callback: given tx data, produce rx data
using SpiResponder = std::function<std::vector<uint8_t>(const uint8_t* tx, size_t len)>;

struct SpiDeviceState {
    spi_device_interface_config_t config{};
    SpiResponder responder;
    int id;
};

class SpiMasterDriver {
public:
    static SpiMasterDriver& instance() {
        static SpiMasterDriver inst;
        return inst;
    }

    esp_err_t busInit(spi_host_device_t host, const spi_bus_config_t* cfg, int dma) {
        (void)host; (void)cfg; (void)dma;
        return ESP_OK;
    }

    esp_err_t addDevice(spi_host_device_t host, const spi_device_interface_config_t* cfg, spi_device_handle_t* handle) {
        (void)host;
        std::lock_guard<std::mutex> lk(mu_);
        auto* dev = new SpiDeviceState();
        dev->config = *cfg;
        dev->id = nextId_++;
        devices_.push_back(dev);
        *handle = (spi_device_handle_t)dev;
        return ESP_OK;
    }

    esp_err_t removeDevice(spi_device_handle_t handle) {
        std::lock_guard<std::mutex> lk(mu_);
        auto* dev = (SpiDeviceState*)handle;
        for (auto it = devices_.begin(); it != devices_.end(); ++it) {
            if (*it == dev) { devices_.erase(it); break; }
        }
        delete dev;
        return ESP_OK;
    }

    esp_err_t transmit(spi_device_handle_t handle, spi_transaction_t* trans) {
        auto* dev = (SpiDeviceState*)handle;
        size_t byteLen = (trans->length + 7) / 8;

        const uint8_t* txData = nullptr;
        if (trans->flags & SPI_TRANS_USE_TXDATA) {
            txData = trans->tx_data;
        } else {
            txData = (const uint8_t*)trans->tx_buffer;
        }

        if (dev->responder && txData) {
            auto rx = dev->responder(txData, byteLen);
            size_t rxLen = (trans->rxlength > 0 ? (trans->rxlength + 7) / 8 : byteLen);
            size_t copyLen = std::min(rx.size(), rxLen);
            if (trans->flags & SPI_TRANS_USE_RXDATA) {
                std::memcpy(trans->rx_data, rx.data(), std::min(copyLen, (size_t)4));
            } else if (trans->rx_buffer) {
                std::memcpy(trans->rx_buffer, rx.data(), copyLen);
            }
        }
        return ESP_OK;
    }

    void setResponder(spi_device_handle_t handle, SpiResponder fn) {
        auto* dev = (SpiDeviceState*)handle;
        dev->responder = std::move(fn);
    }

    void busFree(spi_host_device_t host) { (void)host; }

private:
    std::mutex mu_;
    std::vector<SpiDeviceState*> devices_;
    int nextId_ = 1;
};

} // namespace esp32emu

inline esp_err_t spi_bus_initialize(spi_host_device_t host, const spi_bus_config_t* cfg, int dma) {
    return esp32emu::SpiMasterDriver::instance().busInit(host, cfg, dma);
}

inline esp_err_t spi_bus_add_device(spi_host_device_t host, const spi_device_interface_config_t* cfg, spi_device_handle_t* handle) {
    return esp32emu::SpiMasterDriver::instance().addDevice(host, cfg, handle);
}

inline esp_err_t spi_bus_remove_device(spi_device_handle_t handle) {
    return esp32emu::SpiMasterDriver::instance().removeDevice(handle);
}

inline esp_err_t spi_device_transmit(spi_device_handle_t handle, spi_transaction_t* trans) {
    return esp32emu::SpiMasterDriver::instance().transmit(handle, trans);
}

inline esp_err_t spi_bus_free(spi_host_device_t host) {
    esp32emu::SpiMasterDriver::instance().busFree(host);
    return ESP_OK;
}
