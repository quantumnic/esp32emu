// esp32emu test — AsyncMqttClient
#include "AsyncMqttClient.h"
#include <cassert>
#include <cstdio>

int main() {
    AsyncMqttClient mqtt;

    // Configure
    mqtt.setServer("broker.test", 1883);
    mqtt.setCredentials("user", "pass");
    mqtt.setClientId("test-client");
    mqtt.setKeepAlive(30);
    mqtt.setCleanSession(true);
    assert(mqtt._getHost() == "broker.test");
    assert(mqtt._getPort() == 1883);

    // Callbacks
    bool connected_cb = false;
    bool disconnected_cb = false;
    uint16_t sub_id = 0;
    uint16_t unsub_id = 0;
    std::string recv_topic, recv_payload;

    mqtt.onConnect([&](bool) { connected_cb = true; });
    mqtt.onDisconnect([&](int) { disconnected_cb = true; });
    mqtt.onSubscribe([&](uint16_t id, uint8_t) { sub_id = id; });
    mqtt.onUnsubscribe([&](uint16_t id) { unsub_id = id; });
    mqtt.onMessage([&](const char* t, const char* p, AsyncMqttClientMessageProperties, size_t, size_t, size_t) {
        recv_topic = t;
        recv_payload = p;
    });

    // Connect
    assert(!mqtt.connected());
    mqtt.connect();
    assert(mqtt.connected());
    assert(connected_cb);

    // Subscribe
    uint16_t sid = mqtt.subscribe("test/topic", 1);
    assert(sid > 0);
    assert(sub_id == sid);
    assert(mqtt._getSubscriptions().count("test/topic"));
    assert(mqtt._getSubscriptions().at("test/topic") == 1);

    // Publish
    bool pub_cb = false;
    mqtt.onPublish([&](uint16_t) { pub_cb = true; });
    uint16_t pid = mqtt.publish("test/out", 0, false, "hello", 5);
    assert(pid > 0);
    assert(pub_cb);
    assert(mqtt._getPublished().size() == 1);
    assert(mqtt._getPublished()[0].topic == "test/out");
    assert(mqtt._getPublished()[0].payload == "hello");

    // Inject message
    mqtt._injectMessage("test/topic", "world", 0);
    assert(recv_topic == "test/topic");
    assert(recv_payload == "world");

    // Unsubscribe
    uint16_t uid = mqtt.unsubscribe("test/topic");
    assert(uid > 0);
    assert(unsub_id == uid);
    assert(mqtt._getSubscriptions().empty());

    // Disconnect
    mqtt.disconnect();
    assert(!mqtt.connected());
    assert(disconnected_cb);

    // Reset
    mqtt._reset();
    assert(mqtt._getPublished().empty());

    printf("test_async_mqtt: all assertions passed\n");
    return 0;
}
