#pragma once
// esp32emu — ESP-IDF RingBuffer mock (freertos/ringbuf.h)
// Supports no-split and allow-split ring buffer types.

#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <queue>
#include <mutex>
#include "esp_err.h"

typedef enum {
    RINGBUF_TYPE_NOSPLIT = 0,
    RINGBUF_TYPE_ALLOWSPLIT = 1,
    RINGBUF_TYPE_BYTEBUF = 2,
    RINGBUF_TYPE_MAX
} RingbufferType_t;

typedef void* RingbufHandle_t;

namespace esp32emu {

struct RingbufItem {
    std::vector<uint8_t> data;
};

struct Ringbuffer {
    RingbufferType_t type;
    size_t maxSize;
    size_t usedBytes = 0;
    std::queue<RingbufItem> items;
    // For BYTEBUF mode
    std::vector<uint8_t> byteStream;
    std::mutex mtx;
};

} // namespace esp32emu

inline RingbufHandle_t xRingbufferCreate(size_t bufSize, RingbufferType_t type) {
    auto* rb = new esp32emu::Ringbuffer();
    rb->type = type;
    rb->maxSize = bufSize;
    return static_cast<RingbufHandle_t>(rb);
}

inline void vRingbufferDelete(RingbufHandle_t handle) {
    delete static_cast<esp32emu::Ringbuffer*>(handle);
}

inline int xRingbufferSend(RingbufHandle_t handle, const void* data, size_t len, uint32_t /*ticksToWait*/) {
    auto* rb = static_cast<esp32emu::Ringbuffer*>(handle);
    std::lock_guard<std::mutex> lock(rb->mtx);
    if (rb->usedBytes + len > rb->maxSize) return 0; // pdFALSE
    if (rb->type == RINGBUF_TYPE_BYTEBUF) {
        const uint8_t* p = static_cast<const uint8_t*>(data);
        rb->byteStream.insert(rb->byteStream.end(), p, p + len);
    } else {
        esp32emu::RingbufItem item;
        item.data.assign(static_cast<const uint8_t*>(data),
                         static_cast<const uint8_t*>(data) + len);
        rb->items.push(std::move(item));
    }
    rb->usedBytes += len;
    return 1; // pdTRUE
}

inline void* xRingbufferReceive(RingbufHandle_t handle, size_t* itemSize, uint32_t /*ticksToWait*/) {
    auto* rb = static_cast<esp32emu::Ringbuffer*>(handle);
    std::lock_guard<std::mutex> lock(rb->mtx);
    if (rb->type == RINGBUF_TYPE_BYTEBUF) {
        if (rb->byteStream.empty()) return nullptr;
        size_t sz = rb->byteStream.size();
        void* buf = malloc(sz);
        memcpy(buf, rb->byteStream.data(), sz);
        rb->byteStream.clear();
        rb->usedBytes -= sz;
        if (itemSize) *itemSize = sz;
        return buf;
    }
    if (rb->items.empty()) return nullptr;
    auto& front = rb->items.front();
    size_t sz = front.data.size();
    void* buf = malloc(sz);
    memcpy(buf, front.data.data(), sz);
    rb->items.pop();
    rb->usedBytes -= sz;
    if (itemSize) *itemSize = sz;
    return buf;
}

inline void vRingbufferReturnItem(RingbufHandle_t /*handle*/, void* item) {
    free(item);
}

inline size_t xRingbufferGetCurFreeSize(RingbufHandle_t handle) {
    auto* rb = static_cast<esp32emu::Ringbuffer*>(handle);
    std::lock_guard<std::mutex> lock(rb->mtx);
    return rb->maxSize - rb->usedBytes;
}

// ESP-IDF style static creation (simplified: just allocates dynamically)
inline RingbufHandle_t xRingbufferCreateStatic(size_t bufSize, RingbufferType_t type,
                                                uint8_t* /*buf*/, void* /*staticStruct*/) {
    return xRingbufferCreate(bufSize, type);
}
