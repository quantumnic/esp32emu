// Test: ESP-IDF event loop mock
#include "esp_event.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <atomic>

static std::atomic<int> wifiEvents{0};
static std::atomic<int> ipEvents{0};
static std::atomic<int> anyEvents{0};

static void wifi_handler(void* arg, esp_event_base_t base, int32_t id, void* data) {
    (void)arg; (void)base; (void)id; (void)data;
    wifiEvents++;
}

static void ip_handler(void* arg, esp_event_base_t base, int32_t id, void* data) {
    (void)arg; (void)base; (void)id; (void)data;
    ipEvents++;
}

static void any_handler(void* arg, esp_event_base_t base, int32_t id, void* data) {
    (void)arg; (void)base; (void)id; (void)data;
    anyEvents++;
}

int main() {
    auto& loop = esp32emu::EventLoop::defaultLoop();
    loop.reset();

    assert(esp_event_loop_create_default() == ESP_OK);

    // Register handlers
    assert(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_handler, nullptr) == ESP_OK);
    assert(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, ip_handler, nullptr) == ESP_OK);
    assert(esp_event_handler_register(ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, any_handler, nullptr) == ESP_OK);

    assert(loop.handlerCount() == 3);

    // Post WiFi event
    wifiEvents = 0; ipEvents = 0; anyEvents = 0;
    esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_START, nullptr, 0, 0);
    assert(wifiEvents == 1);
    assert(ipEvents == 0);
    assert(anyEvents == 1); // any catches all

    // Post IP event
    esp_event_post(IP_EVENT, IP_EVENT_STA_GOT_IP, nullptr, 0, 0);
    assert(wifiEvents == 1);
    assert(ipEvents == 1);
    assert(anyEvents == 2);

    // Post IP event with wrong ID - ip_handler shouldn't fire
    esp_event_post(IP_EVENT, IP_EVENT_STA_LOST_IP, nullptr, 0, 0);
    assert(ipEvents == 1); // not matched
    assert(anyEvents == 3); // any still catches

    // Event data passing
    static int receivedData = 0;
    esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED,
        [](void* arg, esp_event_base_t base, int32_t id, void* data) {
            (void)arg; (void)base; (void)id;
            receivedData = *(int*)data;
        }, nullptr);

    int payload = 42;
    esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &payload, sizeof(payload), 0);
    assert(receivedData == 42);

    // Unregister wifi_handler
    int prevWifi = wifiEvents.load();
    esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_handler);
    int prevAny = anyEvents.load();
    esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_START, nullptr, 0, 0);
    // wifi_handler was removed, so wifiEvents should not increase
    assert(wifiEvents == prevWifi);
    // any_handler still catches everything
    assert(anyEvents == prevAny + 1);

    // Cleanup
    assert(esp_event_loop_delete_default() == ESP_OK);
    assert(loop.handlerCount() == 0);

    printf("test_esp_event: all assertions passed\n");
    return 0;
}
