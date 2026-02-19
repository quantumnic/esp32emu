// esp32emu — FreeRTOS Task Notification test
#include "freertos/FreeRTOS.h"
#include <cassert>
#include <cstdio>
#include <atomic>
#include <thread>
#include <chrono>

static std::atomic<bool> g_received{false};
static std::atomic<uint32_t> g_value{0};

static void notifyTakeTask(void* param) {
    TaskHandle_t mainTask = (TaskHandle_t)param;
    // Wait for notification
    uint32_t val = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500));
    g_value = val;
    g_received = true;
    // Notify back
    xTaskNotifyGive(mainTask);
    vTaskDelete(nullptr);
}

static void test_give_take() {
    // We need a TaskRecord for the main thread too
    esp32emu::freertos::TaskRecord mainRec;
    mainRec.name = "main";
    esp32emu::freertos::currentTask = &mainRec;

    g_received = false;
    g_value = 0;

    TaskHandle_t workerHandle = nullptr;
    xTaskCreate(notifyTakeTask, "worker", 4096, (void*)&mainRec, 1, &workerHandle);

    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    // Give 3 times
    xTaskNotifyGive(workerHandle);
    xTaskNotifyGive(workerHandle);
    xTaskNotifyGive(workerHandle);

    // Wait for worker to signal back
    uint32_t val = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(500));
    assert(val > 0);
    assert(g_received.load());
    assert(g_value.load() == 3); // worker got count of 3

    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    esp32emu::freertos::cleanupTasks();
    esp32emu::freertos::currentTask = nullptr;
    printf("test_give_take: passed\n");
}

static std::atomic<uint32_t> g_notifyWaitValue{0};
static std::atomic<bool> g_notifyWaitDone{false};

static void notifyWaitTask(void* param) {
    (void)param;
    uint32_t val = 0;
    BaseType_t r = xTaskNotifyWait(0, 0xFFFFFFFF, &val, pdMS_TO_TICKS(500));
    if (r == pdPASS) {
        g_notifyWaitValue = val;
    }
    g_notifyWaitDone = true;
    vTaskDelete(nullptr);
}

static void test_notify_wait_set_bits() {
    g_notifyWaitValue = 0;
    g_notifyWaitDone = false;

    TaskHandle_t workerHandle = nullptr;
    xTaskCreate(notifyWaitTask, "waiter", 4096, nullptr, 1, &workerHandle);
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    xTaskNotify(workerHandle, 0x05, eSetBits);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    assert(g_notifyWaitDone.load());
    assert(g_notifyWaitValue.load() == 0x05);

    esp32emu::freertos::cleanupTasks();
    printf("test_notify_wait_set_bits: passed\n");
}

static void test_notify_overwrite() {
    esp32emu::freertos::TaskRecord rec;
    rec.name = "test";
    TaskHandle_t h = (TaskHandle_t)&rec;

    xTaskNotify(h, 100, eSetValueWithOverwrite);
    xTaskNotify(h, 200, eSetValueWithOverwrite);

    // Read the value directly
    assert(rec.notification.value == 200);
    printf("test_notify_overwrite: passed\n");
}

static void test_notify_without_overwrite() {
    esp32emu::freertos::TaskRecord rec;
    rec.name = "test2";
    TaskHandle_t h = (TaskHandle_t)&rec;

    BaseType_t r1 = xTaskNotify(h, 100, eSetValueWithoutOverwrite);
    assert(r1 == pdPASS);

    // Second should fail because pending
    BaseType_t r2 = xTaskNotify(h, 200, eSetValueWithoutOverwrite);
    assert(r2 == pdFAIL);
    assert(rec.notification.value == 100);
    printf("test_notify_without_overwrite: passed\n");
}

static void test_get_current_handle() {
    esp32emu::freertos::TaskRecord rec;
    rec.name = "cur";
    esp32emu::freertos::currentTask = &rec;
    assert(xTaskGetCurrentTaskHandle() == (TaskHandle_t)&rec);
    esp32emu::freertos::currentTask = nullptr;
    assert(xTaskGetCurrentTaskHandle() == nullptr);
    printf("test_get_current_handle: passed\n");
}

int main() {
    printf("test_task_notify: starting\n");
    test_give_take();
    test_notify_wait_set_bits();
    test_notify_overwrite();
    test_notify_without_overwrite();
    test_get_current_handle();
    printf("test_task_notify: all assertions passed\n");
    return 0;
}
