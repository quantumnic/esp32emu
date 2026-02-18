#include "esp_task_wdt.h"
#include <cassert>
#include <cstdio>

int main() {
    auto& wdt = esp32emu::TaskWDT::instance();
    wdt.reset();

    // Test 1: init and state
    assert(!wdt.isInitialized());
    esp_task_wdt_init(5000, true);
    assert(wdt.isInitialized());
    assert(wdt.getTimeout() == 5000);

    // Test 2: subscribe and feed
    assert(!wdt.isSubscribed());
    esp_task_wdt_add(nullptr);
    assert(wdt.isSubscribed());

    assert(esp_task_wdt_reset() == ESP_OK);
    assert(wdt.getFeedCount() == 1);
    esp_task_wdt_reset();
    esp_task_wdt_reset();
    assert(wdt.getFeedCount() == 3);

    // Test 3: simulate timeout
    wdt.simulateTimeout();
    assert(wdt.wasTriggered());

    // Test 4: unsubscribe
    esp_task_wdt_delete(nullptr);
    assert(!wdt.isSubscribed());
    // feed should fail when not subscribed
    assert(esp_task_wdt_reset() != ESP_OK);

    // Test 5: deinit
    esp_task_wdt_deinit();
    assert(!wdt.isInitialized());

    printf("test_wdt: all assertions passed\n");
    return 0;
}
