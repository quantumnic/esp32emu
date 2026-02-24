#include "AsyncMqttClient.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Test basic connect/disconnect
    {
        AsyncMqttClient mqtt;
        mqtt._reset();
        bool connectCalled = false;
        bool disconnectCalled = false;

        mqtt.onConnect([&](bool sessionPresent) {
            (void)sessionPresent;
            connectCalled = true;
        });
        mqtt.onDisconnect([&](AsyncMqttClientDisconnectReason reason) {
            (void)reason;
            disconnectCalled = true;
        });

        mqtt.setServer("broker.local", 1883);
        assert(mqtt._getHost() == "broker.local");
        assert(mqtt._getPort() == 1883);

        mqtt.connect();
        assert(mqtt.connected());
        assert(connectCalled);

        mqtt.disconnect();
        assert(!mqtt.connected());
        assert(disconnectCalled);
    }

    // Test credentials and client ID
    {
        AsyncMqttClient mqtt;
        mqtt._reset();
        mqtt.setCredentials("user", "pass123");
        mqtt.setClientId("esp32-sensor-1");
        mqtt.setKeepAlive(30);
        mqtt.setCleanSession(false);
        mqtt.setServer("mqtt.example.com", 8883);
        assert(mqtt._getHost() == "mqtt.example.com");
        assert(mqtt._getPort() == 8883);
    }

    // Test subscribe/unsubscribe
    {
        AsyncMqttClient mqtt;
        mqtt._reset();
        mqtt.connect();

        uint16_t subId = 0;
        mqtt.onSubscribe([&](uint16_t packetId, uint8_t qos) {
            subId = packetId;
            (void)qos;
        });

        uint16_t id = mqtt.subscribe("sensor/temp", 1);
        assert(id > 0);
        assert(subId == id);
        assert(mqtt._getSubscriptions().count("sensor/temp") == 1);
        assert(mqtt._getSubscriptions().at("sensor/temp") == 1);

        bool unsubCalled = false;
        mqtt.onUnsubscribe([&](uint16_t /*packetId*/) {
            unsubCalled = true;
        });
        mqtt.unsubscribe("sensor/temp");
        assert(unsubCalled);
        assert(mqtt._getSubscriptions().count("sensor/temp") == 0);
    }

    // Test publish
    {
        AsyncMqttClient mqtt;
        mqtt._reset();
        mqtt.connect();

        bool publishCalled = false;
        mqtt.onPublish([&](uint16_t /*packetId*/) {
            publishCalled = true;
        });

        mqtt.publish("sensor/temp", 0, false, "23.5");
        assert(publishCalled);
        assert(mqtt._getPublished().size() == 1);
        assert(mqtt._getPublished()[0].topic == "sensor/temp");
        assert(mqtt._getPublished()[0].payload == "23.5");
        assert(mqtt._getPublished()[0].qos == 0);
        assert(mqtt._getPublished()[0].retain == false);

        mqtt.publish("sensor/humidity", 1, true, "65.2");
        assert(mqtt._getPublished().size() == 2);
        assert(mqtt._getPublished()[1].retain == true);
    }

    // Test message injection
    {
        AsyncMqttClient mqtt;
        mqtt._reset();
        mqtt.connect();

        std::string receivedTopic, receivedPayload;
        mqtt.onMessage([&](const char* topic, const char* payload,
                          AsyncMqttClientMessageProperties props,
                          size_t len, size_t index, size_t total) {
            (void)props; (void)index; (void)total;
            receivedTopic = topic;
            receivedPayload = std::string(payload, len);
        });

        mqtt.subscribe("home/#", 0);
        mqtt._injectMessage("home/light", "ON", 0);
        assert(receivedTopic == "home/light");
        assert(receivedPayload == "ON");
    }

    // Test reset
    {
        AsyncMqttClient mqtt;
        mqtt.setServer("host", 1883);
        mqtt.connect();
        mqtt.publish("t", 0, false, "x");
        mqtt.subscribe("s", 0);
        mqtt._reset();
        assert(!mqtt.connected());
        assert(mqtt._getPublished().empty());
        assert(mqtt._getSubscriptions().empty());
    }

    // Test chaining
    {
        AsyncMqttClient mqtt;
        mqtt.setServer("host", 1883)
            .setCredentials("u", "p")
            .setClientId("id")
            .setKeepAlive(120)
            .setCleanSession(true);
        // If we get here, chaining works
    }

    printf("test_asyncmqttclient: all assertions passed\n");
    return 0;
}
