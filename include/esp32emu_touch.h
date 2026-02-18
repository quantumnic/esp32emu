#pragma once
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>

// ESP32 touch pin thresholds
#define TOUCH_PAD_MAX 9  // T0-T9

namespace esp32emu {

class TouchController {
public:
    static TouchController& instance() {
        static TouchController inst;
        return inst;
    }

    // Read touch sensor value (lower = touched)
    uint16_t touchRead(uint8_t pin) {
        std::lock_guard<std::mutex> lk(mtx_);
        auto it = values_.find(pin);
        return (it != values_.end()) ? it->second : default_untouched_;
    }

    // Attach interrupt when touch value drops below threshold
    void touchAttachInterrupt(uint8_t pin, std::function<void()> fn, uint16_t threshold) {
        std::lock_guard<std::mutex> lk(mtx_);
        isrs_[pin] = {fn, threshold};
    }

    // Detach touch interrupt
    void touchDetachInterrupt(uint8_t pin) {
        std::lock_guard<std::mutex> lk(mtx_);
        isrs_.erase(pin);
    }

    // ── Test helpers ──

    // Set touch sensor value (simulates touch when value < threshold)
    void setTouchValue(uint8_t pin, uint16_t value) {
        std::function<void()> callback;
        {
            std::lock_guard<std::mutex> lk(mtx_);
            uint16_t old = values_.count(pin) ? values_[pin] : default_untouched_;
            values_[pin] = value;
            auto it = isrs_.find(pin);
            if (it != isrs_.end() && value < it->second.threshold && old >= it->second.threshold) {
                callback = it->second.fn;
            }
        }
        if (callback) callback();
    }

    // Simulate a touch event: set low value, fire ISR, then restore
    void simulateTouch(uint8_t pin, uint16_t touch_value = 10) {
        setTouchValue(pin, touch_value);
    }

    // Release a touch: restore to untouched value
    void releaseTouch(uint8_t pin) {
        std::lock_guard<std::mutex> lk(mtx_);
        values_[pin] = default_untouched_;
    }

    // Get the threshold configured for a pin
    uint16_t getThreshold(uint8_t pin) {
        std::lock_guard<std::mutex> lk(mtx_);
        auto it = isrs_.find(pin);
        return (it != isrs_.end()) ? it->second.threshold : 0;
    }

    // Get number of ISR triggers (for testing)
    int getTouchCount(uint8_t pin) {
        std::lock_guard<std::mutex> lk(mtx_);
        auto it = touch_counts_.find(pin);
        return (it != touch_counts_.end()) ? it->second : 0;
    }

    void reset() {
        std::lock_guard<std::mutex> lk(mtx_);
        values_.clear();
        isrs_.clear();
        touch_counts_.clear();
    }

private:
    struct TouchISR {
        std::function<void()> fn;
        uint16_t threshold;
    };

    std::mutex mtx_;
    std::map<uint8_t, uint16_t> values_;
    std::map<uint8_t, TouchISR> isrs_;
    std::map<uint8_t, int> touch_counts_;
    uint16_t default_untouched_ = 60;  // typical untouched value ~40-80
};

} // namespace esp32emu

// Arduino-compatible API
inline uint16_t touchRead(uint8_t pin) {
    return esp32emu::TouchController::instance().touchRead(pin);
}
inline void touchAttachInterrupt(uint8_t pin, void(*fn)(), uint16_t threshold) {
    esp32emu::TouchController::instance().touchAttachInterrupt(pin, fn, threshold);
}
inline void touchDetachInterrupt(uint8_t pin) {
    esp32emu::TouchController::instance().touchDetachInterrupt(pin);
}
