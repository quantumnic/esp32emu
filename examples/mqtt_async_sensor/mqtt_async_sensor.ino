// esp32emu example: Async MQTT sensor publisher
// Publishes temperature/humidity to MQTT broker using AsyncMqttClient.

#include <WiFi.h>
#include <AsyncMqttClient.h>

AsyncMqttClient mqttClient;

unsigned long lastPublish = 0;
const unsigned long publishInterval = 5000;

void onMqttConnect(bool sessionPresent) {
    Serial.println("Connected to MQTT broker");
    Serial.printf("Session present: %d\n", sessionPresent);
    mqttClient.subscribe("home/commands", 1);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
    Serial.printf("Disconnected from MQTT, reason: %d\n", reason);
}

void onMqttMessage(const char* topic, const char* payload,
                   AsyncMqttClientMessageProperties properties,
                   size_t len, size_t index, size_t total) {
    (void)properties; (void)index; (void)total;
    Serial.printf("Message on [%s]: %.*s\n", topic, (int)len, payload);
}

void setup() {
    Serial.begin(115200);
    WiFi.begin("MyNetwork", "password");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");

    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    mqttClient.onMessage(onMqttMessage);
    mqttClient.setServer("broker.local", 1883);
    mqttClient.setCredentials("sensor", "s3cur3");
    mqttClient.connect();
}

void loop() {
    if (millis() - lastPublish > publishInterval) {
        float temp = 20.0 + (random(0, 100) / 10.0);
        float hum  = 40.0 + (random(0, 300) / 10.0);
        char buf[32];
        snprintf(buf, sizeof(buf), "%.1f", temp);
        mqttClient.publish("home/sensor/temp", 0, false, buf);
        snprintf(buf, sizeof(buf), "%.1f", hum);
        mqttClient.publish("home/sensor/humidity", 0, false, buf);
        Serial.printf("Published: temp=%.1f, hum=%.1f\n", temp, hum);
        lastPublish = millis();
    }
}
