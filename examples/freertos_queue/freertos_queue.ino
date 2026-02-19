// FreeRTOS Queue example — producer/consumer pattern
// Demonstrates inter-task communication using queues

#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

QueueHandle_t sensorQueue;

struct SensorReading {
    int sensorId;
    float value;
    unsigned long timestamp;
};

void sensorTask(void* param) {
    int id = *(int*)param;
    while (true) {
        SensorReading reading;
        reading.sensorId = id;
        reading.value = random(200, 350) / 10.0f;  // 20.0 - 35.0°C
        reading.timestamp = millis();

        if (xQueueSend(sensorQueue, &reading, pdMS_TO_TICKS(100)) == pdPASS) {
            Serial.print("📡 Sensor ");
            Serial.print(id);
            Serial.print(" sent: ");
            Serial.print(reading.value);
            Serial.println("°C");
        } else {
            Serial.println("⚠️ Queue full!");
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void displayTask(void* param) {
    (void)param;
    SensorReading reading;
    while (true) {
        if (xQueueReceive(sensorQueue, &reading, pdMS_TO_TICKS(1000)) == pdPASS) {
            Serial.print("📊 Display: Sensor ");
            Serial.print(reading.sensorId);
            Serial.print(" = ");
            Serial.print(reading.value);
            Serial.print("°C at t=");
            Serial.println(reading.timestamp);
        }

        UBaseType_t pending = uxQueueMessagesWaiting(sensorQueue);
        if (pending > 0) {
            Serial.print("📬 ");
            Serial.print(pending);
            Serial.println(" readings queued");
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("=== FreeRTOS Queue Demo ===");

    sensorQueue = xQueueCreate(10, sizeof(SensorReading));

    static int id1 = 1, id2 = 2;
    xTaskCreate(sensorTask, "sensor1", 2048, &id1, 1, nullptr);
    xTaskCreate(sensorTask, "sensor2", 2048, &id2, 1, nullptr);
    xTaskCreate(displayTask, "display", 2048, nullptr, 2, nullptr);
}

void loop() {
    delay(5000);
    Serial.print("📈 Queue spaces available: ");
    Serial.println(uxQueueSpacesAvailable(sensorQueue));
}
