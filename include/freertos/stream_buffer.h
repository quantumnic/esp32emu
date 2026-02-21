#pragma once
// esp32emu: FreeRTOS stream buffer mock
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <vector>
#include <mutex>

typedef void* StreamBufferHandle_t;

namespace esp32emu_stream {

struct StreamBuffer {
    std::vector<uint8_t> buf;
    size_t capacity;
    size_t head = 0;
    size_t count = 0;
    std::mutex mtx;

    StreamBuffer(size_t cap) : buf(cap), capacity(cap) {}

    size_t send(const void* data, size_t len) {
        std::lock_guard<std::mutex> lk(mtx);
        size_t avail = capacity - count;
        size_t toWrite = len < avail ? len : avail;
        const uint8_t* src = (const uint8_t*)data;
        for (size_t i = 0; i < toWrite; i++) {
            buf[(head + count + i) % capacity] = src[i];
        }
        count += toWrite;
        return toWrite;
    }

    size_t receive(void* data, size_t len) {
        std::lock_guard<std::mutex> lk(mtx);
        size_t toRead = len < count ? len : count;
        uint8_t* dst = (uint8_t*)data;
        for (size_t i = 0; i < toRead; i++) {
            dst[i] = buf[(head + i) % capacity];
        }
        head = (head + toRead) % capacity;
        count -= toRead;
        return toRead;
    }

    size_t bytesAvailable() {
        std::lock_guard<std::mutex> lk(mtx);
        return count;
    }

    size_t spacesAvailable() {
        std::lock_guard<std::mutex> lk(mtx);
        return capacity - count;
    }

    bool isEmpty() {
        std::lock_guard<std::mutex> lk(mtx);
        return count == 0;
    }

    bool isFull() {
        std::lock_guard<std::mutex> lk(mtx);
        return count == capacity;
    }

    void reset() {
        std::lock_guard<std::mutex> lk(mtx);
        head = 0;
        count = 0;
    }
};

} // namespace esp32emu_stream

inline StreamBufferHandle_t xStreamBufferCreate(size_t xBufferSizeBytes, size_t /*xTriggerLevelBytes*/) {
    return new esp32emu_stream::StreamBuffer(xBufferSizeBytes);
}

inline void vStreamBufferDelete(StreamBufferHandle_t xStreamBuffer) {
    delete static_cast<esp32emu_stream::StreamBuffer*>(xStreamBuffer);
}

inline size_t xStreamBufferSend(StreamBufferHandle_t xStreamBuffer, const void* pvTxData, size_t xDataLengthBytes, uint32_t /*xTicksToWait*/) {
    return static_cast<esp32emu_stream::StreamBuffer*>(xStreamBuffer)->send(pvTxData, xDataLengthBytes);
}

inline size_t xStreamBufferReceive(StreamBufferHandle_t xStreamBuffer, void* pvRxData, size_t xBufferLengthBytes, uint32_t /*xTicksToWait*/) {
    return static_cast<esp32emu_stream::StreamBuffer*>(xStreamBuffer)->receive(pvRxData, xBufferLengthBytes);
}

inline size_t xStreamBufferBytesAvailable(StreamBufferHandle_t xStreamBuffer) {
    return static_cast<esp32emu_stream::StreamBuffer*>(xStreamBuffer)->bytesAvailable();
}

inline size_t xStreamBufferSpacesAvailable(StreamBufferHandle_t xStreamBuffer) {
    return static_cast<esp32emu_stream::StreamBuffer*>(xStreamBuffer)->spacesAvailable();
}

inline int xStreamBufferIsEmpty(StreamBufferHandle_t xStreamBuffer) {
    return static_cast<esp32emu_stream::StreamBuffer*>(xStreamBuffer)->isEmpty() ? 1 : 0;
}

inline int xStreamBufferIsFull(StreamBufferHandle_t xStreamBuffer) {
    return static_cast<esp32emu_stream::StreamBuffer*>(xStreamBuffer)->isFull() ? 1 : 0;
}

inline int xStreamBufferReset(StreamBufferHandle_t xStreamBuffer) {
    static_cast<esp32emu_stream::StreamBuffer*>(xStreamBuffer)->reset();
    return 1; // pdPASS
}

inline size_t xStreamBufferSendFromISR(StreamBufferHandle_t xStreamBuffer, const void* pvTxData, size_t xDataLengthBytes, int* /*pxHigherPriorityTaskWoken*/) {
    return static_cast<esp32emu_stream::StreamBuffer*>(xStreamBuffer)->send(pvTxData, xDataLengthBytes);
}

inline size_t xStreamBufferReceiveFromISR(StreamBufferHandle_t xStreamBuffer, void* pvRxData, size_t xBufferLengthBytes, int* /*pxHigherPriorityTaskWoken*/) {
    return static_cast<esp32emu_stream::StreamBuffer*>(xStreamBuffer)->receive(pvRxData, xBufferLengthBytes);
}

// Message buffer (stream buffer with length prefix)
typedef void* MessageBufferHandle_t;

inline MessageBufferHandle_t xMessageBufferCreate(size_t xBufferSizeBytes) {
    return xStreamBufferCreate(xBufferSizeBytes, 1);
}

inline void vMessageBufferDelete(MessageBufferHandle_t xMessageBuffer) {
    vStreamBufferDelete(xMessageBuffer);
}

inline size_t xMessageBufferSend(MessageBufferHandle_t xMessageBuffer, const void* pvTxData, size_t xDataLengthBytes, uint32_t xTicksToWait) {
    auto* sb = static_cast<esp32emu_stream::StreamBuffer*>(xMessageBuffer);
    // Need space for length prefix + data
    if (sb->spacesAvailable() < xDataLengthBytes + sizeof(uint32_t)) return 0;
    uint32_t len = (uint32_t)xDataLengthBytes;
    sb->send(&len, sizeof(len));
    sb->send(pvTxData, xDataLengthBytes);
    (void)xTicksToWait;
    return xDataLengthBytes;
}

inline size_t xMessageBufferReceive(MessageBufferHandle_t xMessageBuffer, void* pvRxData, size_t xBufferLengthBytes, uint32_t xTicksToWait) {
    auto* sb = static_cast<esp32emu_stream::StreamBuffer*>(xMessageBuffer);
    if (sb->bytesAvailable() < sizeof(uint32_t)) return 0;
    uint32_t len = 0;
    sb->receive(&len, sizeof(len));
    if (len > xBufferLengthBytes) { (void)xTicksToWait; return 0; }
    return sb->receive(pvRxData, len);
}
