// driver/usb_serial_jtag.h — USB Serial/JTAG driver mock for esp32emu
#pragma once
#include "esp_err.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <algorithm>

typedef struct {
    int rx_buffer_size;
    int tx_buffer_size;
} usb_serial_jtag_driver_config_t;

#define USB_SERIAL_JTAG_DRIVER_CONFIG_DEFAULT() { .rx_buffer_size = 256, .tx_buffer_size = 256 }

namespace usb_serial_jtag_test {
    inline bool& installed() { static bool v = false; return v; }
    inline std::vector<uint8_t>& rx_buf() { static std::vector<uint8_t> b; return b; }
    inline std::vector<uint8_t>& tx_buf() { static std::vector<uint8_t> b; return b; }
    inline void reset() { installed() = false; rx_buf().clear(); tx_buf().clear(); }
    inline void injectRx(const void* data, size_t len) {
        auto* p = (const uint8_t*)data;
        rx_buf().insert(rx_buf().end(), p, p + len);
    }
}

inline esp_err_t usb_serial_jtag_driver_install(const usb_serial_jtag_driver_config_t* config) {
    (void)config;
    usb_serial_jtag_test::installed() = true;
    printf("[esp32emu] USB Serial/JTAG: driver installed\n");
    return ESP_OK;
}

inline esp_err_t usb_serial_jtag_driver_uninstall() {
    usb_serial_jtag_test::installed() = false;
    printf("[esp32emu] USB Serial/JTAG: driver uninstalled\n");
    return ESP_OK;
}

inline int usb_serial_jtag_read_bytes(void* buf, uint32_t length, uint32_t /*timeout_ms*/) {
    auto& rx = usb_serial_jtag_test::rx_buf();
    int n = (int)std::min((size_t)length, rx.size());
    if (n > 0) {
        memcpy(buf, rx.data(), n);
        rx.erase(rx.begin(), rx.begin() + n);
    }
    return n;
}

inline int usb_serial_jtag_write_bytes(const void* buf, uint32_t length, uint32_t /*timeout_ms*/) {
    auto* p = (const uint8_t*)buf;
    usb_serial_jtag_test::tx_buf().insert(usb_serial_jtag_test::tx_buf().end(), p, p + length);
    return (int)length;
}
