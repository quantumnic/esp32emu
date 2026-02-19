#include "esp_http_server.h"
#include <cassert>
#include <cstdio>
#include <cstring>

static esp_err_t hello_handler(esp32emu::httpd::Request* req) {
    httpd_resp_set_type(req, "text/plain");
    httpd_resp_send(req, "Hello World", -1);
    return ESP_OK;
}

static esp_err_t echo_handler(esp32emu::httpd::Request* req) {
    char buf[256];
    int len = httpd_req_recv(req, buf, sizeof(buf));
    httpd_resp_send(req, buf, len);
    return ESP_OK;
}

static esp_err_t query_handler(esp32emu::httpd::Request* req) {
    size_t qlen = httpd_req_get_url_query_len(req);
    if (qlen > 0) {
        char query[128];
        httpd_req_get_url_query_str(req, query, sizeof(query));
        httpd_resp_send(req, query, -1);
    } else {
        httpd_resp_send(req, "no query", -1);
    }
    return ESP_OK;
}

int main() {
    // Start server
    httpd_handle_t server = nullptr;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 8080;
    assert(httpd_start(&server, &config) == ESP_OK);
    assert(server != nullptr);

    // Register handlers
    httpd_uri_t hello_uri = { .uri = "/hello", .method = HTTP_GET, .handler = hello_handler, .user_ctx = nullptr };
    assert(httpd_register_uri_handler(server, &hello_uri) == ESP_OK);

    httpd_uri_t echo_uri = { .uri = "/echo", .method = HTTP_POST, .handler = echo_handler, .user_ctx = nullptr };
    assert(httpd_register_uri_handler(server, &echo_uri) == ESP_OK);

    httpd_uri_t query_uri = { .uri = "/search", .method = HTTP_GET, .handler = query_handler, .user_ctx = nullptr };
    assert(httpd_register_uri_handler(server, &query_uri) == ESP_OK);

    // Test dispatch GET /hello
    auto& s = esp32emu::httpd::getServer();
    esp32emu::httpd::Request req;
    assert(s.dispatch(HTTP_GET, "/hello", "", req) == ESP_OK);
    assert(req.response == "Hello World");
    assert(req.content_type == "text/plain");

    // Test dispatch POST /echo
    esp32emu::httpd::Request req2;
    assert(s.dispatch(HTTP_POST, "/echo", "test body", req2) == ESP_OK);
    assert(req2.response == "test body");

    // Test dispatch with query string
    esp32emu::httpd::Request req3;
    assert(s.dispatch(HTTP_GET, "/search?q=esp32", "", req3) == ESP_OK);
    assert(req3.response == "q=esp32");

    // Test 404 (no handler)
    esp32emu::httpd::Request req4;
    assert(s.dispatch(HTTP_GET, "/missing", "", req4) != ESP_OK);

    // Unregister handler
    assert(httpd_unregister_uri_handler(server, "/hello", HTTP_GET) == ESP_OK);
    esp32emu::httpd::Request req5;
    assert(s.dispatch(HTTP_GET, "/hello", "", req5) != ESP_OK); // should fail now

    // Stop server
    assert(httpd_stop(server) == ESP_OK);
    assert(!esp32emu::httpd::getServer().running);

    printf("test_httpd: all assertions passed\n");
    return 0;
}
