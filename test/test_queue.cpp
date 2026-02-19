#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <cassert>
#include <cstdio>
#include <thread>
#include <chrono>

int main() {
    // Basic create/send/receive
    QueueHandle_t q = xQueueCreate(5, sizeof(int));
    assert(q != nullptr);

    int val = 42;
    assert(xQueueSend(q, &val, 0) == pdPASS);
    assert(uxQueueMessagesWaiting(q) == 1);
    assert(uxQueueSpacesAvailable(q) == 4);

    int out = 0;
    assert(xQueueReceive(q, &out, 0) == pdPASS);
    assert(out == 42);
    assert(uxQueueMessagesWaiting(q) == 0);

    // Receive from empty queue (no wait) should fail
    assert(xQueueReceive(q, &out, 0) == pdFAIL);

    // Peek
    val = 99;
    xQueueSend(q, &val, 0);
    assert(xQueuePeek(q, &out, 0) == pdPASS);
    assert(out == 99);
    assert(uxQueueMessagesWaiting(q) == 1); // still there

    // FIFO order
    xQueueReset(q);
    for (int i = 1; i <= 5; i++)
        assert(xQueueSend(q, &i, 0) == pdPASS);
    // Queue full
    val = 6;
    assert(xQueueSend(q, &val, 0) == pdFAIL);

    for (int i = 1; i <= 5; i++) {
        xQueueReceive(q, &out, 0);
        assert(out == i);
    }

    // SendToFront
    xQueueReset(q);
    val = 10;
    xQueueSend(q, &val, 0);
    val = 20;
    xQueueSendToFront(q, &val, 0);
    xQueueReceive(q, &out, 0);
    assert(out == 20); // front item first

    // Cross-thread producer/consumer
    xQueueReset(q);
    std::thread producer([&]{
        for (int i = 0; i < 10; i++) {
            int v = i;
            xQueueSend(q, &v, portMAX_DELAY);
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    int sum = 0;
    for (int i = 0; i < 10; i++) {
        int v;
        assert(xQueueReceive(q, &v, portMAX_DELAY) == pdPASS);
        sum += v;
    }
    assert(sum == 45); // 0+1+...+9

    producer.join();

    // ISR variants
    xQueueReset(q);
    val = 77;
    BaseType_t woken = pdFALSE;
    assert(xQueueSendFromISR(q, &val, &woken) == pdPASS);
    assert(xQueueReceiveFromISR(q, &out, &woken) == pdPASS);
    assert(out == 77);

    vQueueDelete(q);

    printf("test_queue: all assertions passed\n");
    return 0;
}
