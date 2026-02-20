// esp32emu — FreeRTOS RingBuffer test
#include "freertos/ringbuf.h"
#include <cassert>
#include <cstring>
#include <cstdio>

int main() {
    // ── NOSPLIT mode ──
    {
        RingbufHandle_t rb = xRingbufferCreate(256, RINGBUF_TYPE_NOSPLIT);
        assert(rb != nullptr);

        // Send items
        const char* msg1 = "Hello";
        const char* msg2 = "World";
        assert(xRingbufferSend(rb, msg1, strlen(msg1) + 1, 0) == 1);
        assert(xRingbufferSend(rb, msg2, strlen(msg2) + 1, 0) == 1);

        // Free size decreased
        assert(xRingbufferGetCurFreeSize(rb) < 256);

        // Receive in order
        size_t sz = 0;
        void* item = xRingbufferReceive(rb, &sz, 0);
        assert(item != nullptr);
        assert(sz == 6);
        assert(strcmp((char*)item, "Hello") == 0);
        vRingbufferReturnItem(rb, item);

        item = xRingbufferReceive(rb, &sz, 0);
        assert(item != nullptr);
        assert(strcmp((char*)item, "World") == 0);
        vRingbufferReturnItem(rb, item);

        // Empty now
        item = xRingbufferReceive(rb, &sz, 0);
        assert(item == nullptr);

        // Free size restored
        assert(xRingbufferGetCurFreeSize(rb) == 256);

        vRingbufferDelete(rb);
    }

    // ── BYTEBUF mode ──
    {
        RingbufHandle_t rb = xRingbufferCreate(128, RINGBUF_TYPE_BYTEBUF);
        assert(rb != nullptr);

        uint8_t data1[] = {1, 2, 3};
        uint8_t data2[] = {4, 5};
        assert(xRingbufferSend(rb, data1, 3, 0) == 1);
        assert(xRingbufferSend(rb, data2, 2, 0) == 1);

        size_t sz = 0;
        uint8_t* out = (uint8_t*)xRingbufferReceive(rb, &sz, 0);
        assert(out != nullptr);
        assert(sz == 5);
        assert(out[0] == 1 && out[4] == 5);
        vRingbufferReturnItem(rb, out);

        vRingbufferDelete(rb);
    }

    // ── Overflow ──
    {
        RingbufHandle_t rb = xRingbufferCreate(8, RINGBUF_TYPE_NOSPLIT);
        assert(xRingbufferSend(rb, "12345678", 8, 0) == 1);
        assert(xRingbufferSend(rb, "X", 1, 0) == 0); // full
        size_t sz;
        void* item = xRingbufferReceive(rb, &sz, 0);
        assert(item && sz == 8);
        vRingbufferReturnItem(rb, item);
        // Now can send again
        assert(xRingbufferSend(rb, "Y", 1, 0) == 1);
        vRingbufferDelete(rb);
    }

    printf("test_ringbuf: all assertions passed\n");
    return 0;
}
