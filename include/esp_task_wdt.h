#pragma once
#include <cstdint>
#include <atomic>

// ESP32 Task Watchdog Timer mock
// In real hardware, WDT resets the chip if not fed. Here we just track state.

namespace esp32emu {

class TaskWDT {
public:
    static TaskWDT& instance() {
        static TaskWDT inst;
        return inst;
    }

    void init(uint32_t timeout_ms) {
        timeout_ms_ = timeout_ms;
        initialized_ = true;
        fed_ = true;
        triggered_ = false;
        feed_count_ = 0;
    }

    void deinit() {
        initialized_ = false;
    }

    bool add(void* /*task_handle*/ = nullptr) {
        subscribed_ = true;
        return initialized_;
    }

    bool remove(void* /*task_handle*/ = nullptr) {
        subscribed_ = false;
        return true;
    }

    bool feed() {
        if (!initialized_ || !subscribed_) return false;
        fed_ = true;
        feed_count_++;
        return true;
    }

    // Test helpers
    void simulateTimeout() { triggered_ = true; fed_ = false; }
    bool isInitialized() const { return initialized_; }
    bool isSubscribed() const { return subscribed_; }
    bool wasTriggered() const { return triggered_; }
    int getFeedCount() const { return feed_count_; }
    uint32_t getTimeout() const { return timeout_ms_; }

    void reset() {
        initialized_ = false;
        subscribed_ = false;
        fed_ = false;
        triggered_ = false;
        feed_count_ = 0;
        timeout_ms_ = 0;
    }

private:
    std::atomic<bool> initialized_{false};
    std::atomic<bool> subscribed_{false};
    std::atomic<bool> fed_{false};
    std::atomic<bool> triggered_{false};
    std::atomic<int> feed_count_{0};
    uint32_t timeout_ms_ = 0;
};

} // namespace esp32emu

// ESP-IDF compatible API
typedef int esp_err_t;
#ifndef ESP_OK
#define ESP_OK 0
#endif

inline esp_err_t esp_task_wdt_init(uint32_t timeout_ms, bool /*panic*/) {
    esp32emu::TaskWDT::instance().init(timeout_ms);
    return ESP_OK;
}

inline esp_err_t esp_task_wdt_deinit() {
    esp32emu::TaskWDT::instance().deinit();
    return ESP_OK;
}

inline esp_err_t esp_task_wdt_add(void* handle) {
    return esp32emu::TaskWDT::instance().add(handle) ? ESP_OK : -1;
}

inline esp_err_t esp_task_wdt_delete(void* handle) {
    return esp32emu::TaskWDT::instance().remove(handle) ? ESP_OK : -1;
}

inline esp_err_t esp_task_wdt_reset() {
    return esp32emu::TaskWDT::instance().feed() ? ESP_OK : -1;
}
