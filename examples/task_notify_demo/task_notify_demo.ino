// FreeRTOS Task Notification demo — producer/consumer with notify
// Producer gives notifications, consumer takes them

static TaskHandle_t consumerHandle = nullptr;

void consumerTask(void* param) {
    (void)param;
    while (true) {
        uint32_t count = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(2000));
        if (count > 0) {
            Serial.print("Consumer: received ");
            Serial.print(count);
            Serial.println(" notification(s)");
        } else {
            Serial.println("Consumer: timeout, no notifications");
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Task Notification Demo");

    xTaskCreate(consumerTask, "consumer", 4096, nullptr, 1, &consumerHandle);
}

int loopCount = 0;

void loop() {
    delay(300);
    loopCount++;

    // Give notifications at varying rates
    xTaskNotifyGive(consumerHandle);
    Serial.print("Producer: sent notification #");
    Serial.println(loopCount);

    if (loopCount % 3 == 0) {
        // Send extra notification every 3rd cycle
        xTaskNotifyGive(consumerHandle);
        Serial.println("Producer: bonus notification!");
    }
}
