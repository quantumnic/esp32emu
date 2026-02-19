// ESP-IDF native HTTP server example
#include "esp_http_server.h"
#include <cstdio>

static esp_err_t root_handler(esp32emu::httpd::Request* req) {
    const char* html = "<html><body><h1>ESP32 Web Server</h1>"
                       "<p>Running on ESP-IDF httpd</p></body></html>";
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, html, -1);
    return ESP_OK;
}

static esp_err_t api_handler(esp32emu::httpd::Request* req) {
    const char* json = "{\"status\":\"ok\",\"uptime\":12345}";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json, -1);
    return ESP_OK;
}

httpd_handle_t start_webserver() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    httpd_handle_t server = nullptr;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t root = { "/", HTTP_GET, root_handler, nullptr };
        httpd_register_uri_handler(server, &root);

        httpd_uri_t api = { "/api/status", HTTP_GET, api_handler, nullptr };
        httpd_register_uri_handler(server, &api);

        printf("HTTP server started\n");
    }
    return server;
}

int main() {
    auto server = start_webserver();

    // Test dispatch
    auto& s = esp32emu::httpd::getServer();
    esp32emu::httpd::Request req;
    s.dispatch(HTTP_GET, "/api/status", "", req);
    printf("API response: %s\n", req.response.c_str());

    httpd_stop(server);
    return 0;
}
