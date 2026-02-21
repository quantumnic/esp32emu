// Example: FreeRTOS stream buffer producer/consumer pattern
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/stream_buffer.h"
#include <cstdio>
#include <cstring>

StreamBufferHandle_t xStream = nullptr;

void producerTask(void* param) {
    (void)param;
    int counter = 0;
    while (counter < 5) {
        char msg[32];
        snprintf(msg, sizeof(msg), "Data-%d", counter++);
        size_t sent = xStreamBufferSend(xStream, msg, strlen(msg), pdMS_TO_TICKS(100));
        printf("[Producer] Sent %zu bytes: %s\n", sent, msg);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    vTaskDelete(nullptr);
}

void consumerTask(void* param) {
    (void)param;
    int received = 0;
    while (received < 5) {
        char buf[32] = {};
        size_t len = xStreamBufferReceive(xStream, buf, sizeof(buf) - 1, pdMS_TO_TICKS(500));
        if (len > 0) {
            printf("[Consumer] Received %zu bytes: %s\n", len, buf);
            received++;
        }
    }
    vTaskDelete(nullptr);
}

void setup() {
    printf("FreeRTOS Stream Buffer Demo\n");
    xStream = xStreamBufferCreate(128, 1);

    xTaskCreate(producerTask, "producer", 2048, nullptr, 1, nullptr);
    xTaskCreate(consumerTask, "consumer", 2048, nullptr, 1, nullptr);
}

void loop() {
    delay(2000);
    printf("Stream bytes available: %zu\n", xStreamBufferBytesAvailable(xStream));
}
