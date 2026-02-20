#pragma once
// esp32emu — AsyncMqttClient mock
// Popular async MQTT client for ESP32 (marvinroger/async-mqtt-client).

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <string>
#include <vector>
#include <map>

// Forward-declare types
using AsyncMqttClientDisconnectReason = int;
constexpr int TCP_DISCONNECTED = 0;

struct AsyncMqttClientMessageProperties {
    uint8_t qos = 0;
    bool dup = false;
    bool retain = false;
};

class AsyncMqttClient {
public:
    using OnConnectCallback     = std::function<void(bool sessionPresent)>;
    using OnDisconnectCallback  = std::function<void(AsyncMqttClientDisconnectReason reason)>;
    using OnSubscribeCallback   = std::function<void(uint16_t packetId, uint8_t qos)>;
    using OnUnsubscribeCallback = std::function<void(uint16_t packetId)>;
    using OnMessageCallback     = std::function<void(const char* topic, const char* payload,
                                                     AsyncMqttClientMessageProperties properties,
                                                     size_t len, size_t index, size_t total)>;
    using OnPublishCallback     = std::function<void(uint16_t packetId)>;

    AsyncMqttClient& setServer(const char* host, uint16_t port) {
        host_ = host;
        port_ = port;
        return *this;
    }

    AsyncMqttClient& setCredentials(const char* user, const char* pass = nullptr) {
        user_ = user ? user : "";
        pass_ = pass ? pass : "";
        return *this;
    }

    AsyncMqttClient& setClientId(const char* id) {
        client_id_ = id ? id : "";
        return *this;
    }

    AsyncMqttClient& setKeepAlive(uint16_t seconds) {
        keep_alive_ = seconds;
        return *this;
    }

    AsyncMqttClient& setCleanSession(bool clean) {
        clean_session_ = clean;
        return *this;
    }

    AsyncMqttClient& onConnect(OnConnectCallback cb)       { on_connect_ = cb; return *this; }
    AsyncMqttClient& onDisconnect(OnDisconnectCallback cb) { on_disconnect_ = cb; return *this; }
    AsyncMqttClient& onSubscribe(OnSubscribeCallback cb)   { on_subscribe_ = cb; return *this; }
    AsyncMqttClient& onUnsubscribe(OnUnsubscribeCallback cb){ on_unsubscribe_ = cb; return *this; }
    AsyncMqttClient& onMessage(OnMessageCallback cb)       { on_message_ = cb; return *this; }
    AsyncMqttClient& onPublish(OnPublishCallback cb)       { on_publish_ = cb; return *this; }

    void connect() {
        connected_ = true;
        fprintf(stderr, "[esp32emu] AsyncMqtt: connected to %s:%u\n", host_.c_str(), port_);
        if (on_connect_) on_connect_(false);
    }

    void disconnect(bool /*force*/ = false) {
        connected_ = false;
        fprintf(stderr, "[esp32emu] AsyncMqtt: disconnected\n");
        if (on_disconnect_) on_disconnect_(TCP_DISCONNECTED);
    }

    bool connected() const { return connected_; }

    uint16_t subscribe(const char* topic, uint8_t qos) {
        uint16_t id = next_packet_id_++;
        subscriptions_[topic] = qos;
        fprintf(stderr, "[esp32emu] AsyncMqtt: subscribe '%s' qos=%d\n", topic, qos);
        if (on_subscribe_) on_subscribe_(id, qos);
        return id;
    }

    uint16_t unsubscribe(const char* topic) {
        uint16_t id = next_packet_id_++;
        subscriptions_.erase(topic);
        fprintf(stderr, "[esp32emu] AsyncMqtt: unsubscribe '%s'\n", topic);
        if (on_unsubscribe_) on_unsubscribe_(id);
        return id;
    }

    uint16_t publish(const char* topic, uint8_t qos, bool retain,
                     const char* payload, size_t length = 0) {
        uint16_t id = next_packet_id_++;
        size_t len = length > 0 ? length : (payload ? strlen(payload) : 0);
        published_.push_back({topic, payload ? std::string(payload, len) : "", qos, retain});
        fprintf(stderr, "[esp32emu] AsyncMqtt: publish '%s' (%zu bytes)\n", topic, len);
        if (on_publish_) on_publish_(id);
        return id;
    }

    // ---- Test helpers ----
    struct PublishedMessage {
        std::string topic;
        std::string payload;
        uint8_t qos;
        bool retain;
    };

    void _injectMessage(const char* topic, const char* payload, uint8_t qos = 0) {
        if (on_message_) {
            AsyncMqttClientMessageProperties props;
            props.qos = qos;
            size_t len = strlen(payload);
            on_message_(topic, payload, props, len, 0, len);
        }
    }

    const std::vector<PublishedMessage>& _getPublished() const { return published_; }
    const std::map<std::string, uint8_t>& _getSubscriptions() const { return subscriptions_; }
    const std::string& _getHost() const { return host_; }
    uint16_t _getPort() const { return port_; }

    void _reset() {
        connected_ = false;
        published_.clear();
        subscriptions_.clear();
        next_packet_id_ = 1;
    }

private:
    std::string host_;
    uint16_t port_ = 1883;
    std::string user_, pass_, client_id_;
    uint16_t keep_alive_ = 60;
    bool clean_session_ = true;
    bool connected_ = false;
    uint16_t next_packet_id_ = 1;

    std::map<std::string, uint8_t> subscriptions_;
    std::vector<PublishedMessage> published_;

    OnConnectCallback on_connect_;
    OnDisconnectCallback on_disconnect_;
    OnSubscribeCallback on_subscribe_;
    OnUnsubscribeCallback on_unsubscribe_;
    OnMessageCallback on_message_;
    OnPublishCallback on_publish_;
};
