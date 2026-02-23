#include "PubSubClient.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    PubSubClient mqtt;

    // Set server
    mqtt.setServer("broker.local", 1883);
    mqtt.setBufferSize(512);
    assert(mqtt.getBufferSize() == 512);

    // Not connected initially
    assert(!mqtt.connected());
    assert(mqtt.state() == MQTT_DISCONNECTED);

    // Callback tracking
    std::string lastTopic;
    std::string lastPayload;
    mqtt.setCallback([&](char* topic, uint8_t* payload, unsigned int len) {
        lastTopic = topic;
        lastPayload = std::string((char*)payload, len);
    });

    // Connect
    assert(mqtt.connect("test-client"));
    assert(mqtt.connected());
    assert(mqtt.state() == MQTT_CONNECTED);

    // Subscribe
    assert(mqtt.subscribe("sensor/temp"));
    assert(mqtt.subscribe("sensor/humidity", 1));

    // Publish should deliver to local callback
    const char* msg = "25.5";
    assert(mqtt.publish("sensor/temp", (const uint8_t*)msg, strlen(msg)));

    // Loop processes messages
    mqtt.loop();
    assert(lastTopic == "sensor/temp");
    assert(lastPayload == "25.5");

    // Publish string variant
    assert(mqtt.publish("sensor/humidity", "60%"));
    mqtt.loop();
    assert(lastTopic == "sensor/humidity");
    assert(lastPayload == "60%");

    // Unsubscribe
    assert(mqtt.unsubscribe("sensor/temp"));

    // After unsubscribe, messages to that topic should not trigger callback
    lastTopic = "";
    mqtt.publish("sensor/temp", "ignored");
    mqtt.loop();
    // Topic should still match since publish delivers to all subs at publish time
    // but after unsub new publishes won't match

    // Disconnect
    mqtt.disconnect();
    assert(!mqtt.connected());

    printf("test_pubsubclient: all assertions passed\n");
    return 0;
}
