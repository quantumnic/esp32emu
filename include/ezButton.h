#pragma once
// esp32emu — ezButton debounce button library mock

#include <Arduino.h>
#include <cstdint>
#include <cstdio>

class ezButton {
public:
    ezButton(int pin) : pin_(pin), debounce_ms_(50) {
        fprintf(stderr, "[esp32emu] ezButton: pin %d\n", pin);
    }

    void setDebounceTime(unsigned long ms) { debounce_ms_ = ms; }

    int getState() const { return current_state_; }
    int getStateRaw() const { return current_state_; }

    bool isPressed() const { return pressed_; }
    bool isReleased() const { return released_; }

    void setCountMode(int mode) { count_mode_ = mode; }
    unsigned long getCount() const { return count_; }
    void resetCount() { count_ = 0; }

    void loop() {
        bool prev = prev_state_;
        prev_state_ = current_state_;
        pressed_ = (prev == HIGH && current_state_ == LOW);
        released_ = (prev == LOW && current_state_ == HIGH);
        if (count_mode_ == COUNT_FALLING && pressed_) count_++;
        if (count_mode_ == COUNT_RISING && released_) count_++;
        if (count_mode_ == COUNT_BOTH && (pressed_ || released_)) count_++;
    }

    // Test helpers
    void esp32emu_set_state(int state) {
        current_state_ = state;
    }
    void esp32emu_press() {
        current_state_ = LOW;  // active-low
    }
    void esp32emu_release() {
        current_state_ = HIGH;
    }
    int esp32emu_get_pin() const { return pin_; }

    static const int COUNT_FALLING = 0;
    static const int COUNT_RISING  = 1;
    static const int COUNT_BOTH    = 2;

private:
    int pin_;
    unsigned long debounce_ms_;
    int current_state_ = HIGH;
    bool prev_state_ = HIGH;
    bool pressed_ = false;
    bool released_ = false;
    int count_mode_ = COUNT_FALLING;
    unsigned long count_ = 0;
};
