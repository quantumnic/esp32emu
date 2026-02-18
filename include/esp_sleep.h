#pragma once
// esp32emu — ESP32 deep sleep / power management mock
// Instead of exiting, records sleep state for testing.

#include <cstdint>
#include <cstdio>
#include <functional>

typedef enum {
    ESP_SLEEP_WAKEUP_UNDEFINED = 0,
    ESP_SLEEP_WAKEUP_EXT0 = 2,
    ESP_SLEEP_WAKEUP_EXT1 = 3,
    ESP_SLEEP_WAKEUP_TIMER = 4,
    ESP_SLEEP_WAKEUP_TOUCHPAD = 5,
    ESP_SLEEP_WAKEUP_ULP = 6,
    ESP_SLEEP_WAKEUP_GPIO = 7
} esp_sleep_wakeup_cause_t;

namespace esp32emu {

class SleepController {
public:
    static SleepController& instance() {
        static SleepController inst;
        return inst;
    }

    void enableTimerWakeup(uint64_t time_us) {
        timerUs_ = time_us;
        timerSet_ = true;
        fprintf(stderr, "[esp32emu] Sleep timer set: %llu µs\n", (unsigned long long)time_us);
    }

    void enableExt0Wakeup(int pin, int level) {
        ext0Pin_ = pin;
        ext0Level_ = level;
        ext0Set_ = true;
        fprintf(stderr, "[esp32emu] Sleep ext0 wakeup: pin=%d level=%d\n", pin, level);
    }

    void enableExt1Wakeup(uint64_t mask, int mode) {
        ext1Mask_ = mask;
        ext1Mode_ = mode;
        ext1Set_ = true;
    }

    void startDeepSleep() {
        deepSleepCalled_ = true;
        deepSleepCount_++;
        fprintf(stderr, "[esp32emu] Deep sleep started (call #%d)\n", deepSleepCount_);
        if (onSleep_) onSleep_();
    }

    void startLightSleep() {
        lightSleepCalled_ = true;
        lightSleepCount_++;
        fprintf(stderr, "[esp32emu] Light sleep started (call #%d)\n", lightSleepCount_);
        if (onSleep_) onSleep_();
    }

    // ── Test helpers ──
    bool deepSleepCalled() const { return deepSleepCalled_; }
    bool lightSleepCalled() const { return lightSleepCalled_; }
    int deepSleepCount() const { return deepSleepCount_; }
    int lightSleepCount() const { return lightSleepCount_; }
    uint64_t timerUs() const { return timerUs_; }
    bool timerSet() const { return timerSet_; }
    int ext0Pin() const { return ext0Pin_; }
    bool ext0Set() const { return ext0Set_; }

    void setWakeupCause(esp_sleep_wakeup_cause_t cause) { wakeupCause_ = cause; }
    esp_sleep_wakeup_cause_t wakeupCause() const { return wakeupCause_; }

    void onSleep(std::function<void()> cb) { onSleep_ = cb; }

    void reset() {
        timerUs_ = 0; timerSet_ = false;
        ext0Pin_ = -1; ext0Level_ = 0; ext0Set_ = false;
        ext1Mask_ = 0; ext1Mode_ = 0; ext1Set_ = false;
        deepSleepCalled_ = false; lightSleepCalled_ = false;
        deepSleepCount_ = 0; lightSleepCount_ = 0;
        wakeupCause_ = ESP_SLEEP_WAKEUP_UNDEFINED;
        onSleep_ = nullptr;
    }

private:
    uint64_t timerUs_ = 0;
    bool timerSet_ = false;
    int ext0Pin_ = -1;
    int ext0Level_ = 0;
    bool ext0Set_ = false;
    uint64_t ext1Mask_ = 0;
    int ext1Mode_ = 0;
    bool ext1Set_ = false;
    bool deepSleepCalled_ = false;
    bool lightSleepCalled_ = false;
    int deepSleepCount_ = 0;
    int lightSleepCount_ = 0;
    esp_sleep_wakeup_cause_t wakeupCause_ = ESP_SLEEP_WAKEUP_UNDEFINED;
    std::function<void()> onSleep_;
};

} // namespace esp32emu

// ── ESP-IDF style free functions ──
inline void esp_sleep_enable_timer_wakeup(uint64_t time_us) {
    esp32emu::SleepController::instance().enableTimerWakeup(time_us);
}
inline void esp_sleep_enable_ext0_wakeup(int pin, int level) {
    esp32emu::SleepController::instance().enableExt0Wakeup(pin, level);
}
inline void esp_sleep_enable_ext1_wakeup(uint64_t mask, int mode) {
    esp32emu::SleepController::instance().enableExt1Wakeup(mask, mode);
}
inline void esp_deep_sleep_start() {
    esp32emu::SleepController::instance().startDeepSleep();
}
inline void esp_light_sleep_start() {
    esp32emu::SleepController::instance().startLightSleep();
}
inline esp_sleep_wakeup_cause_t esp_sleep_get_wakeup_cause() {
    return esp32emu::SleepController::instance().wakeupCause();
}
