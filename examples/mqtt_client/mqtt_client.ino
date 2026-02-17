// Simulated MQTT Publish/Subscribe
// esp32emu run examples/mqtt_client/mqtt_client.ino
#include "Arduino.h"
#include <map>
#include <vector>
#include <string>
#include <functional>

// ── Simple MQTT Mock ──
class MockMQTT {
public:
    using Callback = std::function<void(const char* topic, const char* payload)>;

    bool connect(const char* broker, int port, const char* clientId) {
        Serial.printf("\033[32m[MQTT] Connected to %s:%d as '%s'\033[0m\n", broker, port, clientId);
        connected_ = true;
        return true;
    }

    void subscribe(const char* topic, Callback cb) {
        subs_[topic] = cb;
        Serial.printf("\033[36m[MQTT] Subscribed: %s\033[0m\n", topic);
    }

    void publish(const char* topic, const char* payload) {
        Serial.printf("\033[33m[MQTT] PUB %s → %s\033[0m\n", topic, payload);
        // Deliver to matching subscribers
        for (auto& [t, cb] : subs_) {
            if (t == topic || matchWildcard(t, topic)) {
                cb(topic, payload);
            }
        }
    }

    bool isConnected() { return connected_; }

private:
    bool connected_ = false;
    std::map<std::string, Callback> subs_;

    bool matchWildcard(const std::string& pattern, const char* topic) {
        if (pattern == "#") return true;
        if (pattern.back() == '#') {
            return std::string(topic).rfind(pattern.substr(0, pattern.size()-1), 0) == 0;
        }
        return false;
    }
};

MockMQTT mqtt;
unsigned long lastPub = 0;
int msgCount = 0;

void onMessage(const char* topic, const char* payload) {
    Serial.printf("\033[35m[MQTT] SUB ← %s: %s\033[0m\n", topic, payload);
}

void setup() {
    Serial.begin(115200);
    Serial.println("📡 MQTT Client Simulation\n");

    mqtt.connect("broker.emulator.local", 1883, "esp32emu-client");
    mqtt.subscribe("home/sensors/#", onMessage);
    mqtt.subscribe("home/commands", onMessage);
}

void loop() {
    if (millis() - lastPub >= 3000) {
        lastPub = millis();
        msgCount++;

        float temp = 20.0 + (random(100) / 10.0);
        float hum = 40.0 + (random(200) / 10.0);

        char payload[64];
        snprintf(payload, sizeof(payload), "{\"temp\":%.1f,\"hum\":%.1f}", temp, hum);
        mqtt.publish("home/sensors/living_room", payload);

        if (msgCount % 3 == 0) {
            mqtt.publish("home/sensors/kitchen", "{\"smoke\":false}");
        }

        if (msgCount % 5 == 0) {
            mqtt.publish("home/commands", "{\"action\":\"toggle_light\"}");
        }
    }

    delay(100);
}
