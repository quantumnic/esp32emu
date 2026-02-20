// Example: ESP-IDF MQTT client — publish sensor data, subscribe to commands
#include "Arduino.h"
#include "mqtt_client.h"

static esp_mqtt_client_handle_t mqtt_client = nullptr;

static void mqtt_event_handler(void * /*args*/, const char * /*base*/, int32_t event_id, void *event_data) {
    auto *event = (esp_mqtt_event_t *)event_data;
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            Serial.println("[MQTT] Connected to broker");
            esp_mqtt_client_subscribe(mqtt_client, "device/cmd", 1);
            break;
        case MQTT_EVENT_DATA:
            Serial.print("[MQTT] Received on ");
            Serial.write(event->topic, event->topic_len);
            Serial.print(": ");
            Serial.write(event->data, event->data_len);
            Serial.println();
            break;
        case MQTT_EVENT_DISCONNECTED:
            Serial.println("[MQTT] Disconnected");
            break;
        default:
            break;
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("MQTT Client Example");

    esp_mqtt_client_config_t cfg{};
    cfg.uri = "mqtt://test.mosquitto.org";
    cfg.event_handle = mqtt_event_handler;
    mqtt_client = esp_mqtt_client_init(&cfg);
    esp_mqtt_client_start(mqtt_client);
}

void loop() {
    static int count = 0;
    char payload[64];
    snprintf(payload, sizeof(payload), "{\"temp\":%.1f,\"count\":%d}", 22.5 + (count % 10) * 0.1, count);
    esp_mqtt_client_publish(mqtt_client, "device/sensor", payload, 0, 1, 0);
    Serial.print("[MQTT] Published: ");
    Serial.println(payload);
    count++;
    delay(2000);
}
