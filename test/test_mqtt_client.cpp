// esp32emu – MQTT client tests
#include "mqtt_client.h"
#include <cassert>
#include <cstdio>
#include <string>
#include <vector>

static std::vector<esp_mqtt_event_id_t> events;
static std::string last_topic, last_data;

static void mqtt_handler(void * /*args*/, const char * /*base*/, int32_t event_id, void *event_data) {
    events.push_back((esp_mqtt_event_id_t)event_id);
    auto *evt = (esp_mqtt_event_t *)event_data;
    if (evt->topic && evt->topic_len > 0) last_topic.assign(evt->topic, evt->topic_len);
    if (evt->data && evt->data_len > 0) last_data.assign(evt->data, evt->data_len);
}

int main() {
    // Init + start
    esp_mqtt_client_config_t cfg{};
    cfg.uri = "mqtt://test.mosquitto.org";
    cfg.event_handle = mqtt_handler;
    auto client = esp_mqtt_client_init(&cfg);
    assert(client != nullptr);

    events.clear();
    assert(esp_mqtt_client_start(client) == 0);
    assert(client->connected);
    assert(!events.empty());
    assert(events.back() == MQTT_EVENT_CONNECTED);

    // Subscribe
    events.clear();
    int msg_id = esp_mqtt_client_subscribe(client, "test/topic", 1);
    assert(msg_id > 0);
    assert(events.back() == MQTT_EVENT_SUBSCRIBED);
    assert(esp32emu::mqtt::get_subscriptions(client).size() == 1);
    assert(esp32emu::mqtt::get_subscriptions(client)[0].topic == "test/topic");

    // Publish
    events.clear();
    msg_id = esp_mqtt_client_publish(client, "test/out", "hello", 5, 0, 0);
    assert(msg_id > 0);
    assert(events.back() == MQTT_EVENT_PUBLISHED);
    auto &pub = esp32emu::mqtt::get_published(client);
    assert(pub.size() == 1);
    assert(pub[0].topic == "test/out");
    assert(pub[0].data == "hello");

    // Inject incoming message
    events.clear();
    last_topic.clear();
    last_data.clear();
    esp32emu::mqtt::inject_message(client, "test/topic", "world");
    assert(events.back() == MQTT_EVENT_DATA);
    assert(last_topic == "test/topic");
    assert(last_data == "world");

    // Unsubscribe
    events.clear();
    esp_mqtt_client_unsubscribe(client, "test/topic");
    assert(events.back() == MQTT_EVENT_UNSUBSCRIBED);
    assert(esp32emu::mqtt::get_subscriptions(client).empty());

    // Disconnect / reconnect
    events.clear();
    esp32emu::mqtt::simulate_disconnect(client);
    assert(!client->connected);
    assert(events.back() == MQTT_EVENT_DISCONNECTED);

    esp32emu::mqtt::simulate_reconnect(client);
    assert(client->connected);
    assert(events.back() == MQTT_EVENT_CONNECTED);

    // Stop + destroy
    esp_mqtt_client_stop(client);
    assert(!client->started);
    esp_mqtt_client_destroy(client);

    printf("test_mqtt_client: all assertions passed\n");
    return 0;
}
