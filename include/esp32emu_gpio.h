#pragma once
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>
#include <vector>

#define NUM_DIGITAL_PINS 40
#define NUM_ANALOG_INPUTS 8

#define RISING  0x01
#define FALLING 0x02
#define CHANGE  0x03

namespace esp32emu {

struct PinState {
    uint8_t mode = 0;    // INPUT/OUTPUT/INPUT_PULLUP
    int digital = 0;     // HIGH/LOW
    int analog = 0;      // 0-4095
};

class GPIOController {
public:
    static GPIOController& instance() {
        static GPIOController inst;
        return inst;
    }

    void pinMode(uint8_t pin, uint8_t mode) {
        std::lock_guard<std::mutex> lk(mtx_);
        pins_[pin].mode = mode;
    }

    void digitalWrite(uint8_t pin, uint8_t val) {
        std::lock_guard<std::mutex> lk(mtx_);
        int old = pins_[pin].digital;
        pins_[pin].digital = val;
        if (old != val) fireISR(pin, old, val);
    }

    int digitalRead(uint8_t pin) {
        std::lock_guard<std::mutex> lk(mtx_);
        return pins_[pin].digital;
    }

    int analogRead(uint8_t pin) {
        std::lock_guard<std::mutex> lk(mtx_);
        return pins_[pin].analog;
    }

    void analogWrite(uint8_t pin, int val) {
        std::lock_guard<std::mutex> lk(mtx_);
        pins_[pin].analog = val;
        pins_[pin].digital = val > 0 ? 1 : 0;
    }

    void setAnalogInput(uint8_t pin, int val) {
        std::lock_guard<std::mutex> lk(mtx_);
        pins_[pin].analog = val;
    }

    void attachInterrupt(uint8_t pin, std::function<void()> fn, int mode) {
        std::lock_guard<std::mutex> lk(mtx_);
        isrs_[pin] = {fn, mode};
    }

    void detachInterrupt(uint8_t pin) {
        std::lock_guard<std::mutex> lk(mtx_);
        isrs_.erase(pin);
    }

    PinState getPin(uint8_t pin) {
        std::lock_guard<std::mutex> lk(mtx_);
        return pins_[pin];
    }

    std::map<uint8_t, PinState> allPins() {
        std::lock_guard<std::mutex> lk(mtx_);
        return pins_;
    }

    void reset() {
        std::lock_guard<std::mutex> lk(mtx_);
        pins_.clear();
        isrs_.clear();
    }

private:
    struct ISREntry { std::function<void()> fn; int mode; };

    void fireISR(uint8_t pin, int old_val, int new_val) {
        auto it = isrs_.find(pin);
        if (it == isrs_.end()) return;
        bool fire = false;
        if (it->second.mode == RISING && old_val == 0 && new_val == 1) fire = true;
        if (it->second.mode == FALLING && old_val == 1 && new_val == 0) fire = true;
        if (it->second.mode == CHANGE) fire = true;
        if (fire && it->second.fn) it->second.fn();
    }

    std::mutex mtx_;
    std::map<uint8_t, PinState> pins_;
    std::map<uint8_t, ISREntry> isrs_;
};

} // namespace esp32emu

inline void pinMode(uint8_t pin, uint8_t mode) {
    esp32emu::GPIOController::instance().pinMode(pin, mode);
}
inline void digitalWrite(uint8_t pin, uint8_t val) {
    esp32emu::GPIOController::instance().digitalWrite(pin, val);
}
inline int digitalRead(uint8_t pin) {
    return esp32emu::GPIOController::instance().digitalRead(pin);
}
inline int analogRead(uint8_t pin) {
    return esp32emu::GPIOController::instance().analogRead(pin);
}
inline void analogWrite(uint8_t pin, int val) {
    esp32emu::GPIOController::instance().analogWrite(pin, val);
}
inline void attachInterrupt(uint8_t pin, void(*fn)(), int mode) {
    esp32emu::GPIOController::instance().attachInterrupt(pin, fn, mode);
}
inline void detachInterrupt(uint8_t pin) {
    esp32emu::GPIOController::instance().detachInterrupt(pin);
}
