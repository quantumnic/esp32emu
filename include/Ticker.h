#pragma once
#include <functional>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <vector>

class Ticker {
public:
    ~Ticker() { detach(); }

    void attach(float seconds, std::function<void()> callback) {
        detach();
        running_ = true;
        interval_ms_ = static_cast<int>(seconds * 1000);
        callback_ = callback;
        thread_ = std::thread([this]() { loop(); });
    }

    void attach_ms(uint32_t milliseconds, std::function<void()> callback) {
        detach();
        running_ = true;
        interval_ms_ = milliseconds;
        callback_ = callback;
        thread_ = std::thread([this]() { loop(); });
    }

    void once(float seconds, std::function<void()> callback) {
        detach();
        running_ = true;
        interval_ms_ = static_cast<int>(seconds * 1000);
        callback_ = callback;
        once_ = true;
        thread_ = std::thread([this]() { loop(); });
    }

    void once_ms(uint32_t milliseconds, std::function<void()> callback) {
        detach();
        running_ = true;
        interval_ms_ = milliseconds;
        callback_ = callback;
        once_ = true;
        thread_ = std::thread([this]() { loop(); });
    }

    void detach() {
        running_ = false;
        if (thread_.joinable()) thread_.join();
        once_ = false;
        fire_count_ = 0;
    }

    bool active() const { return running_; }

    // Test helpers
    int getFireCount() const { return fire_count_; }

private:
    void loop() {
        while (running_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms_));
            if (!running_) break;
            if (callback_) callback_();
            fire_count_++;
            if (once_) { running_ = false; break; }
        }
    }

    std::function<void()> callback_;
    std::thread thread_;
    std::atomic<bool> running_{false};
    std::atomic<bool> once_{false};
    std::atomic<int> fire_count_{0};
    int interval_ms_ = 1000;
};
