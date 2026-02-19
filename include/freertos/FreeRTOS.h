#pragma once
// esp32emu — FreeRTOS mock using std::thread
// Provides lightweight emulation of xTaskCreate, vTaskDelay, mutexes, and semaphores.

#include <cstdint>
#include <cstdio>
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <vector>
#include <atomic>
#include <map>

typedef void* TaskHandle_t;
typedef void (*TaskFunction_t)(void*);
typedef uint32_t TickType_t;
typedef int BaseType_t;
typedef unsigned int UBaseType_t;

#define pdPASS    1
#define pdFAIL    0
#define pdTRUE    1
#define pdFALSE   0
#define portMAX_DELAY 0xFFFFFFFF
#define configMAX_PRIORITIES 25
#define portTICK_PERIOD_MS 1
#define pdMS_TO_TICKS(ms) ((ms) / portTICK_PERIOD_MS)
#define tskIDLE_PRIORITY 0

// Semaphore / Mutex types
typedef void* SemaphoreHandle_t;
typedef void* QueueHandle_t;

// Task notification actions
typedef enum {
    eNoAction = 0,
    eSetBits,
    eIncrement,
    eSetValueWithOverwrite,
    eSetValueWithoutOverwrite
} eNotifyAction;

namespace esp32emu {
namespace freertos {

struct TaskNotification {
    std::mutex mtx;
    std::condition_variable cv;
    uint32_t value{0};
    bool pending{false};
};

struct TaskRecord {
    std::thread thread;
    std::string name;
    std::atomic<bool> running{true};
    TaskNotification notification;
};

// Thread-local current task handle (set in xTaskCreate thread)
inline thread_local TaskRecord* currentTask = nullptr;

inline std::vector<TaskRecord*>& taskList() {
    static std::vector<TaskRecord*> tasks;
    return tasks;
}

inline std::mutex& taskListMutex() {
    static std::mutex mtx;
    return mtx;
}

inline void cleanupTasks() {
    std::vector<TaskRecord*> snapshot;
    {
        std::lock_guard<std::mutex> lock(taskListMutex());
        snapshot.swap(taskList());
    }
    for (auto* t : snapshot) {
        t->running = false;
        if (t->thread.joinable()) t->thread.join();
        delete t;
    }
}

} // namespace freertos
} // namespace esp32emu

inline BaseType_t xTaskCreate(
    TaskFunction_t pvTaskCode,
    const char* pcName,
    uint32_t usStackDepth,
    void* pvParameters,
    UBaseType_t uxPriority,
    TaskHandle_t* pxCreatedTask)
{
    (void)usStackDepth;
    (void)uxPriority;

    auto* rec = new esp32emu::freertos::TaskRecord();
    rec->name = pcName ? pcName : "task";
    rec->thread = std::thread([pvTaskCode, pvParameters, rec]() {
        esp32emu::freertos::currentTask = rec;
        fprintf(stderr, "[esp32emu] Task '%s' started (tid=%p)\n", rec->name.c_str(), (void*)rec);
        pvTaskCode(pvParameters);
        fprintf(stderr, "[esp32emu] Task '%s' ended\n", rec->name.c_str());
    });

    if (pxCreatedTask) *pxCreatedTask = (TaskHandle_t)rec;

    {
        std::lock_guard<std::mutex> lock(esp32emu::freertos::taskListMutex());
        esp32emu::freertos::taskList().push_back(rec);
    }

    fprintf(stderr, "[esp32emu] xTaskCreate: '%s' (priority %u, stack %u)\n", rec->name.c_str(), uxPriority, usStackDepth);
    return pdPASS;
}

inline BaseType_t xTaskCreatePinnedToCore(
    TaskFunction_t pvTaskCode,
    const char* pcName,
    uint32_t usStackDepth,
    void* pvParameters,
    UBaseType_t uxPriority,
    TaskHandle_t* pxCreatedTask,
    int coreId)
{
    (void)coreId;
    fprintf(stderr, "[esp32emu] xTaskCreatePinnedToCore: core=%d (ignored, using host thread)\n", coreId);
    return xTaskCreate(pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask);
}

inline void vTaskDelay(TickType_t xTicksToDelay) {
    std::this_thread::sleep_for(std::chrono::milliseconds(xTicksToDelay * portTICK_PERIOD_MS));
}

inline void vTaskDelete(TaskHandle_t xTask) {
    if (xTask == nullptr) {
        // Delete calling task — just return (thread will end)
        fprintf(stderr, "[esp32emu] vTaskDelete(NULL) — current task ending\n");
        return;
    }
    auto* rec = static_cast<esp32emu::freertos::TaskRecord*>(xTask);
    rec->running = false;
    fprintf(stderr, "[esp32emu] vTaskDelete: '%s'\n", rec->name.c_str());
}

inline TickType_t xTaskGetTickCount() {
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now();
    return (TickType_t)std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
}

inline UBaseType_t uxTaskGetStackHighWaterMark(TaskHandle_t xTask) {
    (void)xTask;
    return 4096; // mock: always plenty of stack
}

// --- Mutex ---
inline SemaphoreHandle_t xSemaphoreCreateMutex() {
    auto* mtx = new std::mutex();
    fprintf(stderr, "[esp32emu] xSemaphoreCreateMutex: %p\n", (void*)mtx);
    return (SemaphoreHandle_t)mtx;
}

inline BaseType_t xSemaphoreTake(SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait) {
    (void)xTicksToWait;
    auto* mtx = static_cast<std::mutex*>(xSemaphore);
    mtx->lock();
    return pdTRUE;
}

inline BaseType_t xSemaphoreGive(SemaphoreHandle_t xSemaphore) {
    auto* mtx = static_cast<std::mutex*>(xSemaphore);
    mtx->unlock();
    return pdTRUE;
}

inline void vSemaphoreDelete(SemaphoreHandle_t xSemaphore) {
    auto* mtx = static_cast<std::mutex*>(xSemaphore);
    delete mtx;
}

// --- Task Current Handle ---
inline TaskHandle_t xTaskGetCurrentTaskHandle() {
    return (TaskHandle_t)esp32emu::freertos::currentTask;
}

// --- Task Notifications ---
inline BaseType_t xTaskNotify(TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction) {
    auto* rec = static_cast<esp32emu::freertos::TaskRecord*>(xTaskToNotify);
    std::lock_guard<std::mutex> lock(rec->notification.mtx);
    switch (eAction) {
        case eSetBits:
            rec->notification.value |= ulValue;
            break;
        case eIncrement:
            rec->notification.value++;
            break;
        case eSetValueWithOverwrite:
            rec->notification.value = ulValue;
            break;
        case eSetValueWithoutOverwrite:
            if (rec->notification.pending) return pdFAIL;
            rec->notification.value = ulValue;
            break;
        case eNoAction:
        default:
            break;
    }
    rec->notification.pending = true;
    rec->notification.cv.notify_all();
    return pdPASS;
}

inline BaseType_t xTaskNotifyGive(TaskHandle_t xTaskToNotify) {
    return xTaskNotify(xTaskToNotify, 0, eIncrement);
}

inline uint32_t ulTaskNotifyTake(BaseType_t xClearCountOnExit, TickType_t xTicksToWait) {
    auto* rec = esp32emu::freertos::currentTask;
    if (!rec) return 0;
    std::unique_lock<std::mutex> lock(rec->notification.mtx);
    if (!rec->notification.pending && xTicksToWait > 0) {
        auto dur = std::chrono::milliseconds(xTicksToWait * portTICK_PERIOD_MS);
        rec->notification.cv.wait_for(lock, dur, [rec]{ return rec->notification.pending; });
    }
    if (!rec->notification.pending) return 0;
    uint32_t val = rec->notification.value;
    if (xClearCountOnExit) {
        rec->notification.value = 0;
    } else {
        if (rec->notification.value > 0) rec->notification.value--;
    }
    rec->notification.pending = (rec->notification.value > 0);
    return val;
}

inline BaseType_t xTaskNotifyWait(uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit,
                                   uint32_t* pulNotificationValue, TickType_t xTicksToWait) {
    auto* rec = esp32emu::freertos::currentTask;
    if (!rec) return pdFAIL;
    std::unique_lock<std::mutex> lock(rec->notification.mtx);
    rec->notification.value &= ~ulBitsToClearOnEntry;
    if (!rec->notification.pending && xTicksToWait > 0) {
        auto dur = std::chrono::milliseconds(xTicksToWait * portTICK_PERIOD_MS);
        rec->notification.cv.wait_for(lock, dur, [rec]{ return rec->notification.pending; });
    }
    if (!rec->notification.pending) return pdFAIL;
    if (pulNotificationValue) *pulNotificationValue = rec->notification.value;
    rec->notification.value &= ~ulBitsToClearOnExit;
    rec->notification.pending = false;
    return pdPASS;
}
