// test_stream_buffer: FreeRTOS stream buffer mock tests
#include "freertos/stream_buffer.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Basic create/delete
    StreamBufferHandle_t sb = xStreamBufferCreate(64, 1);
    assert(sb != nullptr);
    assert(xStreamBufferIsEmpty(sb));
    assert(!xStreamBufferIsFull(sb));
    assert(xStreamBufferBytesAvailable(sb) == 0);
    assert(xStreamBufferSpacesAvailable(sb) == 64);

    // Send and receive
    const char* msg = "Hello";
    size_t sent = xStreamBufferSend(sb, msg, 5, 0);
    assert(sent == 5);
    assert(xStreamBufferBytesAvailable(sb) == 5);
    assert(!xStreamBufferIsEmpty(sb));

    char buf[32] = {};
    size_t recv = xStreamBufferReceive(sb, buf, sizeof(buf), 0);
    assert(recv == 5);
    assert(memcmp(buf, "Hello", 5) == 0);
    assert(xStreamBufferIsEmpty(sb));

    // Fill to capacity
    uint8_t data[64];
    memset(data, 0xAA, 64);
    sent = xStreamBufferSend(sb, data, 64, 0);
    assert(sent == 64);
    assert(xStreamBufferIsFull(sb));
    assert(xStreamBufferSpacesAvailable(sb) == 0);

    // Overflow: can't send more
    uint8_t extra = 0xBB;
    sent = xStreamBufferSend(sb, &extra, 1, 0);
    assert(sent == 0);

    // Partial read
    uint8_t out[10];
    recv = xStreamBufferReceive(sb, out, 10, 0);
    assert(recv == 10);
    assert(xStreamBufferBytesAvailable(sb) == 54);

    // Reset
    xStreamBufferReset(sb);
    assert(xStreamBufferIsEmpty(sb));
    assert(xStreamBufferSpacesAvailable(sb) == 64);

    // ISR variants
    sent = xStreamBufferSendFromISR(sb, msg, 5, nullptr);
    assert(sent == 5);
    memset(buf, 0, sizeof(buf));
    recv = xStreamBufferReceiveFromISR(sb, buf, sizeof(buf), nullptr);
    assert(recv == 5);
    assert(memcmp(buf, "Hello", 5) == 0);

    vStreamBufferDelete(sb);

    // Message buffer
    MessageBufferHandle_t mb = xMessageBufferCreate(128);
    assert(mb != nullptr);

    const char* m1 = "First";
    const char* m2 = "Second";
    size_t s1 = xMessageBufferSend(mb, m1, 5, 0);
    assert(s1 == 5);
    size_t s2 = xMessageBufferSend(mb, m2, 6, 0);
    assert(s2 == 6);

    char rbuf[32] = {};
    size_t r1 = xMessageBufferReceive(mb, rbuf, sizeof(rbuf), 0);
    assert(r1 == 5);
    assert(memcmp(rbuf, "First", 5) == 0);

    memset(rbuf, 0, sizeof(rbuf));
    size_t r2 = xMessageBufferReceive(mb, rbuf, sizeof(rbuf), 0);
    assert(r2 == 6);
    assert(memcmp(rbuf, "Second", 6) == 0);

    vMessageBufferDelete(mb);

    printf("test_stream_buffer: all assertions passed\n");
    return 0;
}
