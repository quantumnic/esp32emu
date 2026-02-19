#pragma once
// esp32emu — FreeRTOS Software Timer mock
// xTimerCreate, xTimerStart, xTimerStop, xTimerReset, xTimerChangePeriod, xTimerDelete
// Backed by std::thread + condition_variable for periodic/one-shot callbacks.

#include "freertos/FreeRTOS.h"
#include <condition_variable>
#include <atomic>
#include <thread>
#include <mutex>
#include <chrono>
#include <cstdio>

typedef void* TimerHandle_t;
typedef void (*TimerCallbackFunction_t)(TimerHandle_t);

namespace esp32emu {
namespace freertos_timer {

struct SoftTimer {
    std::string name;
    TickType_t periodTicks;
    bool autoReload;
    void* timerId;
    TimerCallbackFunction_t callback;

    std::thread worker;
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> running{false};
    std::atomic<bool> deleted{false};
    std::atomic<uint32_t> fireCount{0};

    void start() {
        if (running.load()) return;
        if (worker.joinable()) worker.join();
        running = true;
        worker = std::thread([this]() {
            while (running.load() && !deleted.load()) {
                auto deadline = std::chrono::steady_clock::now() +
                    std::chrono::milliseconds(periodTicks * portTICK_PERIOD_MS);

                // Sleep in small increments so we can respond to stop quickly
                while (std::chrono::steady_clock::now() < deadline) {
                    if (!running.load() || deleted.load()) return;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }

                if (!running.load() || deleted.load()) return;
                fireCount++;
                callback((TimerHandle_t)this);
                if (!autoReload) {
                    running = false;
                    return;
                }
            }
        });
    }

    void stop() {
        running = false;
        if (worker.joinable()) worker.join();
    }

    void destroy() {
        deleted = true;
        stop();
    }
};

} // namespace freertos_timer
} // namespace esp32emu

inline TimerHandle_t xTimerCreate(
    const char* pcTimerName,
    TickType_t xTimerPeriodInTicks,
    BaseType_t uxAutoReload,
    void* pvTimerID,
    TimerCallbackFunction_t pxCallbackFunction)
{
    auto* t = new esp32emu::freertos_timer::SoftTimer();
    t->name = pcTimerName ? pcTimerName : "timer";
    t->periodTicks = xTimerPeriodInTicks;
    t->autoReload = (uxAutoReload == pdTRUE);
    t->timerId = pvTimerID;
    t->callback = pxCallbackFunction;
    fprintf(stderr, "[esp32emu] xTimerCreate: '%s' period=%u auto=%d\n",
            t->name.c_str(), xTimerPeriodInTicks, uxAutoReload);
    return (TimerHandle_t)t;
}

inline BaseType_t xTimerStart(TimerHandle_t xTimer, TickType_t xTicksToWait) {
    (void)xTicksToWait;
    auto* t = static_cast<esp32emu::freertos_timer::SoftTimer*>(xTimer);
    t->start();
    return pdPASS;
}

inline BaseType_t xTimerStop(TimerHandle_t xTimer, TickType_t xTicksToWait) {
    (void)xTicksToWait;
    auto* t = static_cast<esp32emu::freertos_timer::SoftTimer*>(xTimer);
    t->stop();
    return pdPASS;
}

inline BaseType_t xTimerReset(TimerHandle_t xTimer, TickType_t xTicksToWait) {
    auto* t = static_cast<esp32emu::freertos_timer::SoftTimer*>(xTimer);
    t->stop();
    return xTimerStart(xTimer, xTicksToWait);
}

inline BaseType_t xTimerChangePeriod(TimerHandle_t xTimer, TickType_t xNewPeriod, TickType_t xTicksToWait) {
    auto* t = static_cast<esp32emu::freertos_timer::SoftTimer*>(xTimer);
    bool wasRunning = t->running.load();
    if (wasRunning) t->stop();
    t->periodTicks = xNewPeriod;
    if (wasRunning) return xTimerStart(xTimer, xTicksToWait);
    return pdPASS;
}

inline BaseType_t xTimerDelete(TimerHandle_t xTimer, TickType_t xTicksToWait) {
    (void)xTicksToWait;
    auto* t = static_cast<esp32emu::freertos_timer::SoftTimer*>(xTimer);
    t->destroy();
    delete t;
    return pdPASS;
}

inline void* pvTimerGetTimerID(TimerHandle_t xTimer) {
    auto* t = static_cast<esp32emu::freertos_timer::SoftTimer*>(xTimer);
    return t->timerId;
}

inline const char* pcTimerGetName(TimerHandle_t xTimer) {
    auto* t = static_cast<esp32emu::freertos_timer::SoftTimer*>(xTimer);
    return t->name.c_str();
}

inline BaseType_t xTimerIsTimerActive(TimerHandle_t xTimer) {
    auto* t = static_cast<esp32emu::freertos_timer::SoftTimer*>(xTimer);
    return t->running.load() ? pdTRUE : pdFALSE;
}

// ISR variants (same in emulation)
inline BaseType_t xTimerStartFromISR(TimerHandle_t xTimer, BaseType_t* pxHigherPriorityTaskWoken) {
    if (pxHigherPriorityTaskWoken) *pxHigherPriorityTaskWoken = pdFALSE;
    return xTimerStart(xTimer, 0);
}

inline BaseType_t xTimerStopFromISR(TimerHandle_t xTimer, BaseType_t* pxHigherPriorityTaskWoken) {
    if (pxHigherPriorityTaskWoken) *pxHigherPriorityTaskWoken = pdFALSE;
    return xTimerStop(xTimer, 0);
}

inline BaseType_t xTimerResetFromISR(TimerHandle_t xTimer, BaseType_t* pxHigherPriorityTaskWoken) {
    if (pxHigherPriorityTaskWoken) *pxHigherPriorityTaskWoken = pdFALSE;
    return xTimerReset(xTimer, 0);
}

// Test helpers
namespace timer_test {
    inline uint32_t getFireCount(TimerHandle_t xTimer) {
        auto* t = static_cast<esp32emu::freertos_timer::SoftTimer*>(xTimer);
        return t->fireCount.load();
    }
    inline void resetFireCount(TimerHandle_t xTimer) {
        auto* t = static_cast<esp32emu::freertos_timer::SoftTimer*>(xTimer);
        t->fireCount = 0;
    }
}
