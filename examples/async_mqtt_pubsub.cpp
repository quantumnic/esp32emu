// esp32emu example — Async MQTT Publish/Subscribe
// Uses AsyncMqttClient for non-blocking MQTT communication.
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncMqttClient.h>

AsyncMqttClient mqttClient;

void onMqttConnect(bool sessionPresent) {
    Serial.println("Connected to MQTT broker!");
    Serial.print("Session present: "); Serial.println(sessionPresent);

    mqttClient.subscribe("esp32/command", 1);
    mqttClient.publish("esp32/status", 0, true, "online");
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    Serial.print("Disconnected from MQTT. Reason: "); Serial.println(reason);
}

void onMqttMessage(const char* topic, const char* payload,
                   AsyncMqttClientMessageProperties properties,
                   size_t len, size_t index, size_t total) {
    Serial.print("Message on ["); Serial.print(topic); Serial.print("]: ");
    Serial.write((const uint8_t*)payload, len);
    Serial.println();
}

void onMqttPublish(uint16_t packetId) {
    Serial.print("Publish ACK, packetId: "); Serial.println(packetId);
}

void setup() {
    Serial.begin(115200);
    Serial.println("Async MQTT Example");

    mqttClient.setServer("broker.hivemq.com", 1883);
    mqttClient.setClientId("esp32-async");
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);

    WiFi.begin("TestNetwork", "password123");
    while (WiFi.status() != WL_CONNECTED) delay(500);
    Serial.println("WiFi connected");

    mqttClient.connect();
}

static unsigned long lastPublish = 0;

void loop() {
    if (millis() - lastPublish > 5000) {
        lastPublish = millis();
        String msg = "uptime=" + String(millis() / 1000);
        mqttClient.publish("esp32/telemetry", 0, false, msg.c_str());
    }
    delay(100);
}
