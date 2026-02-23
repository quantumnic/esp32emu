#include "esp_task_wdt.h"
#include <cassert>
#include <cstdio>

int main() {
    auto& wdt = esp32emu::TaskWDT::instance();
    wdt.reset();

    // Not initialized
    assert(!wdt.isInitialized());
    assert(!wdt.isSubscribed());

    // Init
    assert(esp_task_wdt_init(5000, true) == ESP_OK);
    assert(wdt.isInitialized());
    assert(wdt.getTimeout() == 5000);

    // Add task
    assert(esp_task_wdt_add(nullptr) == ESP_OK);
    assert(wdt.isSubscribed());

    // Feed
    assert(esp_task_wdt_reset() == ESP_OK);
    assert(wdt.getFeedCount() == 1);
    assert(esp_task_wdt_reset() == ESP_OK);
    assert(wdt.getFeedCount() == 2);

    // Simulate timeout
    wdt.simulateTimeout();
    assert(wdt.wasTriggered());

    // Remove task
    assert(esp_task_wdt_delete(nullptr) == ESP_OK);
    assert(!wdt.isSubscribed());

    // Feed without subscription fails
    assert(esp_task_wdt_reset() != ESP_OK);

    // Deinit
    assert(esp_task_wdt_deinit() == ESP_OK);
    assert(!wdt.isInitialized());

    // Feed without init fails
    wdt.reset();
    assert(esp_task_wdt_reset() != ESP_OK);

    printf("test_task_wdt: all assertions passed\n");
    return 0;
}
