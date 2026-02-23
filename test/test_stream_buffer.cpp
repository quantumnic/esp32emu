#include "freertos/stream_buffer.h"
#include <cassert>
#include <cstring>
#include <cstdio>

int main() {
    // Create
    StreamBufferHandle_t sb = xStreamBufferCreate(64, 1);
    assert(sb != nullptr);
    assert(xStreamBufferIsEmpty(sb) == 1);
    assert(xStreamBufferIsFull(sb) == 0);
    assert(xStreamBufferSpacesAvailable(sb) == 64);
    assert(xStreamBufferBytesAvailable(sb) == 0);

    // Send and receive
    const char* msg = "Hello";
    size_t sent = xStreamBufferSend(sb, msg, 5, 0);
    assert(sent == 5);
    assert(xStreamBufferBytesAvailable(sb) == 5);
    assert(xStreamBufferSpacesAvailable(sb) == 59);
    assert(xStreamBufferIsEmpty(sb) == 0);

    char buf[32] = {};
    size_t received = xStreamBufferReceive(sb, buf, sizeof(buf), 0);
    assert(received == 5);
    assert(memcmp(buf, "Hello", 5) == 0);
    assert(xStreamBufferIsEmpty(sb) == 1);

    // Fill to capacity
    char big[64];
    memset(big, 'A', 64);
    sent = xStreamBufferSend(sb, big, 64, 0);
    assert(sent == 64);
    assert(xStreamBufferIsFull(sb) == 1);
    assert(xStreamBufferSpacesAvailable(sb) == 0);

    // Partial receive
    char out[10];
    received = xStreamBufferReceive(sb, out, 10, 0);
    assert(received == 10);
    assert(xStreamBufferBytesAvailable(sb) == 54);

    // Reset
    xStreamBufferReset(sb);
    assert(xStreamBufferIsEmpty(sb) == 1);
    assert(xStreamBufferBytesAvailable(sb) == 0);

    vStreamBufferDelete(sb);

    printf("test_stream_buffer: all assertions passed\n");
    return 0;
}
