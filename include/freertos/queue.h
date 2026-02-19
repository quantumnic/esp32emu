#pragma once
// esp32emu — FreeRTOS Queue mock using std::deque + std::mutex
// Supports xQueueCreate, xQueueSend, xQueueReceive, xQueuePeek, uxQueueMessagesWaiting

#include "FreeRTOS.h"
#include <deque>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include <cstdio>
#include <chrono>
#include <vector>

namespace esp32emu {
namespace freertos {

struct QueueData {
    size_t itemSize;
    size_t maxLength;
    std::deque<std::vector<uint8_t>> items;
    std::mutex mtx;
    std::condition_variable cvNotEmpty;
    std::condition_variable cvNotFull;
};

} // namespace freertos
} // namespace esp32emu

inline QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength, UBaseType_t uxItemSize) {
    auto* q = new esp32emu::freertos::QueueData();
    q->itemSize = uxItemSize;
    q->maxLength = uxQueueLength;
    fprintf(stderr, "[esp32emu] xQueueCreate: length=%u itemSize=%u\n", uxQueueLength, uxItemSize);
    return (QueueHandle_t)q;
}

inline BaseType_t xQueueSend(QueueHandle_t xQueue, const void* pvItemToQueue, TickType_t xTicksToWait) {
    auto* q = static_cast<esp32emu::freertos::QueueData*>(xQueue);
    std::unique_lock<std::mutex> lock(q->mtx);

    if (q->items.size() >= q->maxLength) {
        if (xTicksToWait == 0) return pdFAIL;
        auto timeout = (xTicksToWait == portMAX_DELAY)
            ? std::chrono::milliseconds(60000)
            : std::chrono::milliseconds(xTicksToWait * portTICK_PERIOD_MS);
        if (!q->cvNotFull.wait_for(lock, timeout, [&]{ return q->items.size() < q->maxLength; }))
            return pdFAIL;
    }

    std::vector<uint8_t> item(q->itemSize);
    std::memcpy(item.data(), pvItemToQueue, q->itemSize);
    q->items.push_back(std::move(item));
    q->cvNotEmpty.notify_one();
    return pdPASS;
}

inline BaseType_t xQueueSendToBack(QueueHandle_t xQueue, const void* pvItemToQueue, TickType_t xTicksToWait) {
    return xQueueSend(xQueue, pvItemToQueue, xTicksToWait);
}

inline BaseType_t xQueueSendToFront(QueueHandle_t xQueue, const void* pvItemToQueue, TickType_t xTicksToWait) {
    auto* q = static_cast<esp32emu::freertos::QueueData*>(xQueue);
    std::unique_lock<std::mutex> lock(q->mtx);

    if (q->items.size() >= q->maxLength) {
        if (xTicksToWait == 0) return pdFAIL;
        auto timeout = (xTicksToWait == portMAX_DELAY)
            ? std::chrono::milliseconds(60000)
            : std::chrono::milliseconds(xTicksToWait * portTICK_PERIOD_MS);
        if (!q->cvNotFull.wait_for(lock, timeout, [&]{ return q->items.size() < q->maxLength; }))
            return pdFAIL;
    }

    std::vector<uint8_t> item(q->itemSize);
    std::memcpy(item.data(), pvItemToQueue, q->itemSize);
    q->items.push_front(std::move(item));
    q->cvNotEmpty.notify_one();
    return pdPASS;
}

inline BaseType_t xQueueReceive(QueueHandle_t xQueue, void* pvBuffer, TickType_t xTicksToWait) {
    auto* q = static_cast<esp32emu::freertos::QueueData*>(xQueue);
    std::unique_lock<std::mutex> lock(q->mtx);

    if (q->items.empty()) {
        if (xTicksToWait == 0) return pdFAIL;
        auto timeout = (xTicksToWait == portMAX_DELAY)
            ? std::chrono::milliseconds(60000)
            : std::chrono::milliseconds(xTicksToWait * portTICK_PERIOD_MS);
        if (!q->cvNotEmpty.wait_for(lock, timeout, [&]{ return !q->items.empty(); }))
            return pdFAIL;
    }

    std::memcpy(pvBuffer, q->items.front().data(), q->itemSize);
    q->items.pop_front();
    q->cvNotFull.notify_one();
    return pdPASS;
}

inline BaseType_t xQueuePeek(QueueHandle_t xQueue, void* pvBuffer, TickType_t xTicksToWait) {
    auto* q = static_cast<esp32emu::freertos::QueueData*>(xQueue);
    std::unique_lock<std::mutex> lock(q->mtx);

    if (q->items.empty()) {
        if (xTicksToWait == 0) return pdFAIL;
        auto timeout = (xTicksToWait == portMAX_DELAY)
            ? std::chrono::milliseconds(60000)
            : std::chrono::milliseconds(xTicksToWait * portTICK_PERIOD_MS);
        if (!q->cvNotEmpty.wait_for(lock, timeout, [&]{ return !q->items.empty(); }))
            return pdFAIL;
    }

    std::memcpy(pvBuffer, q->items.front().data(), q->itemSize);
    return pdPASS;
}

inline UBaseType_t uxQueueMessagesWaiting(QueueHandle_t xQueue) {
    auto* q = static_cast<esp32emu::freertos::QueueData*>(xQueue);
    std::lock_guard<std::mutex> lock(q->mtx);
    return (UBaseType_t)q->items.size();
}

inline UBaseType_t uxQueueSpacesAvailable(QueueHandle_t xQueue) {
    auto* q = static_cast<esp32emu::freertos::QueueData*>(xQueue);
    std::lock_guard<std::mutex> lock(q->mtx);
    return (UBaseType_t)(q->maxLength - q->items.size());
}

inline void vQueueDelete(QueueHandle_t xQueue) {
    auto* q = static_cast<esp32emu::freertos::QueueData*>(xQueue);
    delete q;
}

inline BaseType_t xQueueReset(QueueHandle_t xQueue) {
    auto* q = static_cast<esp32emu::freertos::QueueData*>(xQueue);
    std::lock_guard<std::mutex> lock(q->mtx);
    q->items.clear();
    return pdPASS;
}

// ISR variants (no actual ISR context in emulator, just forward)
inline BaseType_t xQueueSendFromISR(QueueHandle_t xQueue, const void* pvItemToQueue, BaseType_t* pxHigherPriorityTaskWoken) {
    if (pxHigherPriorityTaskWoken) *pxHigherPriorityTaskWoken = pdFALSE;
    return xQueueSend(xQueue, pvItemToQueue, 0);
}

inline BaseType_t xQueueReceiveFromISR(QueueHandle_t xQueue, void* pvBuffer, BaseType_t* pxHigherPriorityTaskWoken) {
    if (pxHigherPriorityTaskWoken) *pxHigherPriorityTaskWoken = pdFALSE;
    return xQueueReceive(xQueue, pvBuffer, 0);
}
