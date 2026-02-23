// ESP-IDF WebSocket Client Example
// Connects to a WebSocket server and exchanges messages

#include <Arduino.h>
#include <WiFi.h>
#include "esp_websocket_client.h"
#include "esp_log.h"

static const char *TAG = "ws_client";

static void ws_event_handler(void *args, const char *base, int32_t event_id, void *event_data) {
    (void)args; (void)base;
    switch (event_id) {
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI(TAG, "WebSocket connected");
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "WebSocket disconnected");
            break;
        case WEBSOCKET_EVENT_DATA: {
            esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
            ESP_LOGI(TAG, "Received: %.*s", data->data_len, data->data_ptr);
            break;
        }
        case WEBSOCKET_EVENT_ERROR:
            ESP_LOGE(TAG, "WebSocket error");
            break;
        default:
            break;
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("ESP-IDF WebSocket Client Example");
    Serial.println("================================");

    WiFi.begin("MyNetwork", "password");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    Serial.println("WiFi connected");

    esp_websocket_client_config_t cfg = {};
    cfg.uri = "ws://echo.websocket.org";

    esp_websocket_client_handle_t client = esp_websocket_client_init(&cfg);
    esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, ws_event_handler, NULL);
    esp_websocket_client_start(client);

    Serial.println("WebSocket client started");

    // Send a message
    const char *msg = "Hello from ESP32!";
    esp_websocket_client_send_text(client, msg, strlen(msg), 5000);
    Serial.print("Sent: ");
    Serial.println(msg);
}

void loop() {
    delay(5000);
    Serial.println("WebSocket client running...");
}
