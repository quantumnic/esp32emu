#include "esp_websocket_client.h"
#include "esp_err.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>

static std::string last_data;

static void ws_handler(void *args, const char *base, int32_t event_id, void *event_data) {
    (void)args; (void)base;
    if (event_id == WEBSOCKET_EVENT_DATA) {
        auto *ev = (esp_websocket_event_data_t *)event_data;
        last_data = std::string(ev->data_ptr, ev->data_len);
    }
}

int main() {
    esp_websocket_client_config_t cfg = {};
    cfg.uri = "ws://localhost:8080/ws";

    esp_websocket_client_handle_t client = esp_websocket_client_init(&cfg);
    assert(client != nullptr);

    // Register handler
    int err = esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, ws_handler, nullptr);
    assert(err == ESP_OK);

    // Start
    err = esp_websocket_client_start(client);
    assert(err == ESP_OK);
    assert(esp_websocket_client_is_connected(client));

    // Send text
    int sent = esp_websocket_client_send_text(client, "hello", 5, 1000);
    assert(sent == 5);

    // Verify sent log
    auto &log = esp_websocket_test_get_sent();
    assert(!log.empty());
    assert(log.back().data == "hello");
    assert(log.back().opcode == 1);

    // Inject and fire received data
    client->inject_data("world");
    // Fire event manually
    client->fire_event(WEBSOCKET_EVENT_DATA, "world", 5, 1);
    assert(last_data == "world");

    // Send binary
    uint8_t bin[] = {0x01, 0x02, 0x03};
    sent = esp_websocket_client_send_bin(client, (const char*)bin, 3, 1000);
    assert(sent == 3);

    // Stop
    err = esp_websocket_client_stop(client);
    assert(err == ESP_OK);
    assert(!esp_websocket_client_is_connected(client));

    // Destroy
    err = esp_websocket_client_destroy(client);
    assert(err == ESP_OK);

    // Clean up
    esp_websocket_test_clear();

    printf("test_websocket_client: all assertions passed\n");
    return 0;
}
