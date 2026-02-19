#pragma once
// esp32emu: ESP-IDF HTTP server mock (httpd)
// Lightweight mock for esp_http_server.h — register handlers, test routing.

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <functional>
#include <cstdio>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ESP_OK
#define ESP_OK 0
#endif

typedef int esp_err_t;
typedef void* httpd_handle_t;
typedef int httpd_req_t_handle;

typedef enum {
    HTTP_GET     = 0,
    HTTP_POST    = 1,
    HTTP_PUT     = 2,
    HTTP_DELETE  = 3,
    HTTP_PATCH   = 4,
} httpd_method_t;

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

namespace esp32emu {
namespace httpd {

struct Request {
    httpd_method_t method = HTTP_GET;
    std::string uri;
    std::string body;
    std::string response;
    int status_code = 200;
    std::string content_type = "text/html";
};

} // namespace httpd
} // namespace esp32emu

typedef esp_err_t (*httpd_handler_func_t)(esp32emu::httpd::Request* req);

struct httpd_uri_t {
    const char* uri;
    httpd_method_t method;
    httpd_handler_func_t handler;
    void* user_ctx;
};

struct httpd_config_t {
    uint16_t server_port = 80;
    uint16_t ctrl_port = 32768;
    uint16_t max_open_sockets = 7;
    uint16_t max_uri_handlers = 8;
    uint16_t max_resp_headers = 8;
    uint16_t backlog_conn = 5;
    size_t stack_size = 4096;
    unsigned task_priority = 5;
};

namespace esp32emu {
namespace httpd {

struct ServerState {
    bool running = false;
    httpd_config_t config{};
    std::vector<httpd_uri_t> handlers;

    // Test helper: dispatch a fake request
    esp_err_t dispatch(httpd_method_t method, const std::string& uri, const std::string& body, Request& req) {
        req.method = method;
        req.uri = uri;
        req.body = body;
        for (auto& h : handlers) {
            // Match path part only (before '?')
        std::string path = uri;
        auto qpos = path.find('?');
        if (qpos != std::string::npos) path = path.substr(0, qpos);
        if (h.method == method && path == h.uri) {
                return h.handler(&req);
            }
        }
        return -1; // not found
    }
};

inline ServerState& getServer() {
    static ServerState s;
    return s;
}

} // namespace httpd
} // namespace esp32emu

inline httpd_config_t HTTPD_DEFAULT_CONFIG() {
    return httpd_config_t{};
}

inline esp_err_t httpd_start(httpd_handle_t* handle, const httpd_config_t* config) {
    auto& s = esp32emu::httpd::getServer();
    s.running = true;
    s.config = *config;
    s.handlers.clear();
    *handle = &s;
    printf("[esp32emu] httpd started on port %d\n", config->server_port);
    return ESP_OK;
}

inline esp_err_t httpd_stop(httpd_handle_t handle) {
    (void)handle;
    auto& s = esp32emu::httpd::getServer();
    s.running = false;
    s.handlers.clear();
    printf("[esp32emu] httpd stopped\n");
    return ESP_OK;
}

inline esp_err_t httpd_register_uri_handler(httpd_handle_t handle, const httpd_uri_t* uri_handler) {
    (void)handle;
    auto& s = esp32emu::httpd::getServer();
    s.handlers.push_back(*uri_handler);
    return ESP_OK;
}

inline esp_err_t httpd_unregister_uri_handler(httpd_handle_t handle, const char* uri, httpd_method_t method) {
    (void)handle;
    auto& s = esp32emu::httpd::getServer();
    for (auto it = s.handlers.begin(); it != s.handlers.end(); ++it) {
        if (it->method == method && strcmp(it->uri, uri) == 0) {
            s.handlers.erase(it);
            return ESP_OK;
        }
    }
    return -1;
}

// Response helpers for use inside handlers
inline esp_err_t httpd_resp_send(esp32emu::httpd::Request* req, const char* buf, ssize_t len) {
    req->response = (len < 0) ? std::string(buf) : std::string(buf, (size_t)len);
    return ESP_OK;
}

inline esp_err_t httpd_resp_set_type(esp32emu::httpd::Request* req, const char* type) {
    req->content_type = type;
    return ESP_OK;
}

inline esp_err_t httpd_resp_set_status(esp32emu::httpd::Request* req, const char* status) {
    req->status_code = atoi(status);
    return ESP_OK;
}

inline esp_err_t httpd_resp_send_404(esp32emu::httpd::Request* req) {
    req->status_code = 404;
    req->response = "Not Found";
    return ESP_OK;
}

inline size_t httpd_req_get_url_query_len(esp32emu::httpd::Request* req) {
    auto pos = req->uri.find('?');
    if (pos == std::string::npos) return 0;
    return req->uri.size() - pos - 1;
}

inline esp_err_t httpd_req_get_url_query_str(esp32emu::httpd::Request* req, char* buf, size_t buf_len) {
    auto pos = req->uri.find('?');
    if (pos == std::string::npos) return -1;
    std::string query = req->uri.substr(pos + 1);
    if (query.size() >= buf_len) return -1;
    memcpy(buf, query.c_str(), query.size() + 1);
    return ESP_OK;
}

inline int httpd_req_recv(esp32emu::httpd::Request* req, char* buf, size_t buf_len) {
    size_t to_copy = std::min(buf_len, req->body.size());
    memcpy(buf, req->body.data(), to_copy);
    return (int)to_copy;
}

#endif
