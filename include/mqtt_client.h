#pragma once
// esp32emu – ESP-IDF MQTT client mock
// Emulates esp_mqtt_client with event callbacks, publish/subscribe, QoS.

#include <cstdint>
#include <cstring>
#include <functional>
#include <map>
#include <mutex>
#include <string>
#include <vector>

// ── Event types ─────────────────────────────────────────────────────
typedef enum {
    MQTT_EVENT_ANY = -1,
    MQTT_EVENT_ERROR = 0,
    MQTT_EVENT_CONNECTED,
    MQTT_EVENT_DISCONNECTED,
    MQTT_EVENT_SUBSCRIBED,
    MQTT_EVENT_UNSUBSCRIBED,
    MQTT_EVENT_PUBLISHED,
    MQTT_EVENT_DATA,
    MQTT_EVENT_BEFORE_CONNECT,
    MQTT_EVENT_DELETED,
} esp_mqtt_event_id_t;

typedef enum {
    MQTT_TRANSPORT_UNKNOWN = 0,
    MQTT_TRANSPORT_OVER_TCP,
    MQTT_TRANSPORT_OVER_SSL,
    MQTT_TRANSPORT_OVER_WS,
    MQTT_TRANSPORT_OVER_WSS,
} esp_mqtt_transport_t;

// Forward declarations
struct esp_mqtt_client;
typedef struct esp_mqtt_client *esp_mqtt_client_handle_t;

// ── Event data ──────────────────────────────────────────────────────
typedef struct {
    esp_mqtt_event_id_t event_id;
    esp_mqtt_client_handle_t client;
    char *data;
    int data_len;
    int total_data_len;
    int current_data_offset;
    char *topic;
    int topic_len;
    int msg_id;
    int session_present;
    int retain;
    int qos;
    bool dup;
} esp_mqtt_event_t;

typedef esp_mqtt_event_t *esp_mqtt_event_handle_t;

// Event handler function type
typedef void (*esp_mqtt_event_handler_t)(void *handler_args, const char *base, int32_t event_id, void *event_data);

// ── Configuration ───────────────────────────────────────────────────
typedef struct {
    const char *uri;
    const char *host;
    uint32_t port;
    const char *username;
    const char *password;
    const char *client_id;
    const char *lwt_topic;
    const char *lwt_msg;
    int lwt_qos;
    int lwt_retain;
    int lwt_msg_len;
    int keepalive;
    bool disable_clean_session;
    esp_mqtt_transport_t transport;
    int buffer_size;
    int out_buffer_size;
    // Event handler
    esp_mqtt_event_handler_t event_handle;
    void *user_context;
} esp_mqtt_client_config_t;

// ── Subscription record ─────────────────────────────────────────────
struct mqtt_subscription_t {
    std::string topic;
    int qos;
};

// ── Published message record (for test inspection) ──────────────────
struct mqtt_published_msg_t {
    std::string topic;
    std::string data;
    int qos;
    int retain;
    int msg_id;
};

// ── Client state ────────────────────────────────────────────────────
struct esp_mqtt_client {
    esp_mqtt_client_config_t config{};
    std::string uri;
    std::string client_id;
    bool connected = false;
    bool started = false;
    int next_msg_id = 1;
    std::vector<mqtt_subscription_t> subscriptions;
    std::vector<mqtt_published_msg_t> published;
    esp_mqtt_event_handler_t event_handler = nullptr;
    void *user_context = nullptr;
    std::mutex mtx;

    void fire_event(esp_mqtt_event_id_t id, const char *topic = nullptr, const char *data = nullptr, int data_len = 0, int msg_id = 0) {
        if (!event_handler) return;
        esp_mqtt_event_t evt{};
        evt.event_id = id;
        evt.client = this;
        evt.msg_id = msg_id;
        std::string topic_buf, data_buf;
        if (topic) {
            topic_buf = topic;
            evt.topic = const_cast<char*>(topic_buf.c_str());
            evt.topic_len = (int)topic_buf.size();
        }
        if (data) {
            data_buf = std::string(data, data_len > 0 ? data_len : strlen(data));
            evt.data = const_cast<char*>(data_buf.c_str());
            evt.data_len = (int)data_buf.size();
            evt.total_data_len = evt.data_len;
        }
        event_handler(user_context, "MQTT_EVENTS", (int32_t)id, &evt);
    }
};

// ── API ─────────────────────────────────────────────────────────────
inline esp_mqtt_client_handle_t esp_mqtt_client_init(const esp_mqtt_client_config_t *config) {
    auto *client = new esp_mqtt_client();
    if (config) {
        client->config = *config;
        if (config->uri) client->uri = config->uri;
        if (config->client_id) client->client_id = config->client_id;
        client->event_handler = config->event_handle;
        client->user_context = config->user_context;
    }
    return client;
}

inline int esp_mqtt_client_start(esp_mqtt_client_handle_t client) {
    if (!client) return -1;
    std::lock_guard<std::mutex> lk(client->mtx);
    client->started = true;
    client->connected = true;
    client->fire_event(MQTT_EVENT_CONNECTED);
    return 0;
}

inline int esp_mqtt_client_stop(esp_mqtt_client_handle_t client) {
    if (!client) return -1;
    std::lock_guard<std::mutex> lk(client->mtx);
    if (client->connected) {
        client->connected = false;
        client->fire_event(MQTT_EVENT_DISCONNECTED);
    }
    client->started = false;
    return 0;
}

inline int esp_mqtt_client_destroy(esp_mqtt_client_handle_t client) {
    if (!client) return -1;
    delete client;
    return 0;
}

inline int esp_mqtt_client_subscribe(esp_mqtt_client_handle_t client, const char *topic, int qos) {
    if (!client || !topic) return -1;
    std::lock_guard<std::mutex> lk(client->mtx);
    int msg_id = client->next_msg_id++;
    client->subscriptions.push_back({topic, qos});
    client->fire_event(MQTT_EVENT_SUBSCRIBED, nullptr, nullptr, 0, msg_id);
    return msg_id;
}

inline int esp_mqtt_client_unsubscribe(esp_mqtt_client_handle_t client, const char *topic) {
    if (!client || !topic) return -1;
    std::lock_guard<std::mutex> lk(client->mtx);
    int msg_id = client->next_msg_id++;
    std::string t(topic);
    client->subscriptions.erase(
        std::remove_if(client->subscriptions.begin(), client->subscriptions.end(),
                       [&](const mqtt_subscription_t &s) { return s.topic == t; }),
        client->subscriptions.end());
    client->fire_event(MQTT_EVENT_UNSUBSCRIBED, nullptr, nullptr, 0, msg_id);
    return msg_id;
}

inline int esp_mqtt_client_publish(esp_mqtt_client_handle_t client, const char *topic, const char *data, int len, int qos, int retain) {
    if (!client || !topic) return -1;
    std::lock_guard<std::mutex> lk(client->mtx);
    int msg_id = client->next_msg_id++;
    int actual_len = (data && len > 0) ? len : (data ? (int)strlen(data) : 0);
    std::string payload = data ? std::string(data, actual_len) : "";
    client->published.push_back({topic, payload, qos, retain, msg_id});
    client->fire_event(MQTT_EVENT_PUBLISHED, nullptr, nullptr, 0, msg_id);
    return msg_id;
}

inline int esp_mqtt_client_enqueue(esp_mqtt_client_handle_t client, const char *topic, const char *data, int len, int qos, int retain, bool /*store*/) {
    return esp_mqtt_client_publish(client, topic, data, len, qos, retain);
}

// ── Test helpers ────────────────────────────────────────────────────
namespace esp32emu {
namespace mqtt {

// Inject an incoming message (simulates broker delivering a message)
inline void inject_message(esp_mqtt_client_handle_t client, const char *topic, const char *data, int data_len = 0) {
    if (!client) return;
    int len = data_len > 0 ? data_len : (data ? (int)strlen(data) : 0);
    client->fire_event(MQTT_EVENT_DATA, topic, data, len, 0);
}

// Get published messages
inline const std::vector<mqtt_published_msg_t> &get_published(esp_mqtt_client_handle_t client) {
    return client->published;
}

// Get subscriptions
inline const std::vector<mqtt_subscription_t> &get_subscriptions(esp_mqtt_client_handle_t client) {
    return client->subscriptions;
}

// Clear published messages
inline void clear_published(esp_mqtt_client_handle_t client) {
    std::lock_guard<std::mutex> lk(client->mtx);
    client->published.clear();
}

// Simulate disconnect
inline void simulate_disconnect(esp_mqtt_client_handle_t client) {
    if (!client) return;
    client->connected = false;
    client->fire_event(MQTT_EVENT_DISCONNECTED);
}

// Simulate reconnect
inline void simulate_reconnect(esp_mqtt_client_handle_t client) {
    if (!client) return;
    client->connected = true;
    client->fire_event(MQTT_EVENT_CONNECTED);
}

} // namespace mqtt
} // namespace esp32emu
