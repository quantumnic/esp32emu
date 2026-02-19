// esp32emu — FreeRTOS Software Timer test
#include "freertos/timers.h"
#include <cassert>
#include <cstdio>
#include <atomic>
#include <thread>
#include <chrono>

static std::atomic<int> g_periodic_count{0};
static std::atomic<int> g_oneshot_count{0};

static void periodicCallback(TimerHandle_t xTimer) {
    (void)xTimer;
    g_periodic_count++;
}

static void oneshotCallback(TimerHandle_t xTimer) {
    (void)xTimer;
    g_oneshot_count++;
}

static void test_create_start_stop() {
    g_periodic_count = 0;
    auto t = xTimerCreate("test", pdMS_TO_TICKS(30), pdTRUE, nullptr, periodicCallback);
    assert(t != nullptr);
    assert(xTimerIsTimerActive(t) == pdFALSE);

    xTimerStart(t, 0);
    assert(xTimerIsTimerActive(t) == pdTRUE);

    // 30ms period, wait 200ms → expect 4-7 fires (generous bounds for CI)
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    xTimerStop(t, 0);

    int count = g_periodic_count.load();
    assert(count >= 2 && count <= 10);

    xTimerDelete(t, 0);
    printf("test_create_start_stop: passed (fired %d times)\n", count);
}

static void test_one_shot() {
    g_oneshot_count = 0;
    auto t = xTimerCreate("oneshot", pdMS_TO_TICKS(30), pdFALSE, nullptr, oneshotCallback);
    xTimerStart(t, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    assert(g_oneshot_count.load() == 1);
    assert(xTimerIsTimerActive(t) == pdFALSE);

    xTimerDelete(t, 0);
    printf("test_one_shot: passed\n");
}

static void test_change_period() {
    g_periodic_count = 0;
    auto t = xTimerCreate("chg", pdMS_TO_TICKS(500), pdTRUE, nullptr, periodicCallback);
    xTimerStart(t, 0);
    // Change to 30ms — should fire multiple times in 200ms
    xTimerChangePeriod(t, pdMS_TO_TICKS(30), 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    xTimerStop(t, 0);

    int count = g_periodic_count.load();
    assert(count >= 2);
    xTimerDelete(t, 0);
    printf("test_change_period: passed (fired %d times)\n", count);
}

static void test_timer_id_and_name() {
    int myId = 42;
    auto t = xTimerCreate("named", pdMS_TO_TICKS(100), pdFALSE, &myId, periodicCallback);
    assert(pvTimerGetTimerID(t) == &myId);
    assert(std::string(pcTimerGetName(t)) == "named");
    xTimerDelete(t, 0);
    printf("test_timer_id_and_name: passed\n");
}

static void test_reset() {
    g_oneshot_count = 0;
    // Create one-shot timer, let it fire, then reset to fire again
    auto t = xTimerCreate("rst", pdMS_TO_TICKS(30), pdFALSE, nullptr, oneshotCallback);
    xTimerStart(t, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    assert(g_oneshot_count.load() == 1);
    assert(xTimerIsTimerActive(t) == pdFALSE);

    // Reset should restart the timer
    xTimerReset(t, 0);
    assert(xTimerIsTimerActive(t) == pdTRUE);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    assert(g_oneshot_count.load() == 2);

    xTimerDelete(t, 0);
    printf("test_reset: passed\n");
}

static void test_fire_count_helper() {
    auto t = xTimerCreate("fc", pdMS_TO_TICKS(30), pdTRUE, nullptr, periodicCallback);
    assert(timer_test::getFireCount(t) == 0);
    xTimerStart(t, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    xTimerStop(t, 0);
    assert(timer_test::getFireCount(t) >= 2);
    timer_test::resetFireCount(t);
    assert(timer_test::getFireCount(t) == 0);
    xTimerDelete(t, 0);
    printf("test_fire_count_helper: passed\n");
}

int main() {
    printf("test_freertos_timers: starting\n");
    test_create_start_stop();
    test_one_shot();
    test_change_period();
    test_timer_id_and_name();
    test_reset();
    test_fire_count_helper();
    printf("test_freertos_timers: all assertions passed\n");
    return 0;
}
