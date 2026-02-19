// Example: ESP-IDF event loop – WiFi connection simulation
// Demonstrates esp_event mock
#include <Arduino.h>
#include "esp_event.h"
#include <cstdio>

static bool connected = false;

static void wifi_event_handler(void* arg, esp_event_base_t base, int32_t id, void* data) {
    (void)arg; (void)data;
    if (id == WIFI_EVENT_STA_START) {
        printf("[event_loop] WiFi STA started, connecting...\n");
    } else if (id == WIFI_EVENT_STA_CONNECTED) {
        printf("[event_loop] WiFi connected!\n");
    } else if (id == WIFI_EVENT_STA_DISCONNECTED) {
        printf("[event_loop] WiFi disconnected, reconnecting...\n");
    }
}

static void ip_event_handler(void* arg, esp_event_base_t base, int32_t id, void* data) {
    (void)arg; (void)data;
    if (id == IP_EVENT_STA_GOT_IP) {
        printf("[event_loop] Got IP address!\n");
        connected = true;
    }
}

void setup() {
    esp_event_loop_create_default();

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, nullptr);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, ip_event_handler, nullptr);

    printf("[event_loop] Event handlers registered, simulating WiFi connection...\n");

    // Simulate connection sequence
    esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_START, nullptr, 0, 0);
    esp_event_post(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, nullptr, 0, 0);
    esp_event_post(IP_EVENT, IP_EVENT_STA_GOT_IP, nullptr, 0, 0);
}

void loop() {
    if (connected) {
        printf("[event_loop] System running with network\n");
    }
}
