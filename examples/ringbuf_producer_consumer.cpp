// Example: FreeRTOS RingBuffer producer-consumer pattern
// Common pattern for passing data between tasks on ESP32.
#include <Arduino.h>
#include "freertos/ringbuf.h"
#include <cstdio>
#include <cstring>

RingbufHandle_t sensorBuf = nullptr;

struct SensorReading {
    uint32_t timestamp;
    float temperature;
    float humidity;
};

void producerTask() {
    SensorReading reading = {
        .timestamp = millis(),
        .temperature = 22.5f,
        .humidity = 55.0f
    };

    if (xRingbufferSend(sensorBuf, &reading, sizeof(reading), pdMS_TO_TICKS(100))) {
        Serial.printf("Sent: t=%.1f h=%.1f\n", reading.temperature, reading.humidity);
    } else {
        Serial.println("Ring buffer full!");
    }
}

void consumerTask() {
    size_t itemSize = 0;
    void* item = xRingbufferReceive(sensorBuf, &itemSize, pdMS_TO_TICKS(100));

    if (item && itemSize == sizeof(SensorReading)) {
        auto* r = static_cast<SensorReading*>(item);
        Serial.printf("Received: ts=%u t=%.1f h=%.1f\n",
                      r->timestamp, r->temperature, r->humidity);
        vRingbufferReturnItem(sensorBuf, item);
    }
}

void setup() {
    Serial.begin(115200);

    sensorBuf = xRingbufferCreate(1024, RINGBUF_TYPE_NOSPLIT);
    if (!sensorBuf) {
        Serial.println("Failed to create ring buffer!");
        return;
    }

    Serial.printf("Ring buffer created, free: %zu bytes\n",
                  xRingbufferGetCurFreeSize(sensorBuf));

    // Simulate producer-consumer cycle
    for (int i = 0; i < 5; i++) {
        producerTask();
    }
    for (int i = 0; i < 5; i++) {
        consumerTask();
    }

    vRingbufferDelete(sensorBuf);
    Serial.println("Done.");
}

void loop() {
    delay(1000);
}

#ifdef ESP32EMU_TEST
int main() {
    setup();
    return 0;
}
#endif
