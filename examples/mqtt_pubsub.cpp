// MQTT PubSubClient Example
// Connects to MQTT broker and publishes sensor data

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

unsigned long lastMsg = 0;
int msgCount = 0;

void callback(char* topic, uint8_t* payload, unsigned int length) {
    Serial.print("Message [");
    Serial.print(topic);
    Serial.print("]: ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void reconnect() {
    while (!mqtt.connected()) {
        Serial.print("Connecting to MQTT...");
        if (mqtt.connect("esp32-client")) {
            Serial.println("connected!");
            mqtt.subscribe("esp32/command");
        } else {
            Serial.print("failed, rc=");
            Serial.println(mqtt.state());
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("MQTT PubSubClient Example");
    Serial.println("=========================");

    WiFi.begin("MyNetwork", "password");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");

    mqtt.setServer("broker.local", 1883);
    mqtt.setCallback(callback);
}

void loop() {
    if (!mqtt.connected()) {
        reconnect();
    }
    mqtt.loop();

    unsigned long now = millis();
    if (now - lastMsg > 5000) {
        lastMsg = now;
        msgCount++;
        char msg[64];
        snprintf(msg, sizeof(msg), "{\"temp\":%.1f,\"count\":%d}", 22.5 + (msgCount % 10) * 0.5, msgCount);
        Serial.print("Publish: ");
        Serial.println(msg);
        mqtt.publish("esp32/sensor", msg);
    }
}
