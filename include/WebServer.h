#pragma once
#include <string>
#include <map>
#include <functional>
#include <vector>
#include <cstdint>
#include "esp32emu_string.h"

// Forward declaration — implementation in src/webserver.cpp
class WebServer {
public:
    using THandlerFunction = std::function<void()>;

    explicit WebServer(uint16_t port = 80);
    ~WebServer();

    void begin();
    void close();
    void stop() { close(); }
    void handleClient();

    // Route registration
    void on(const char* uri, THandlerFunction handler);
    void on(const char* uri, int method, THandlerFunction handler);
    void onNotFound(THandlerFunction handler);

    // Request info
    String uri() const;
    int method() const;
    String arg(const char* name) const;
    String arg(int i) const;
    String argName(int i) const;
    int args() const;
    bool hasArg(const char* name) const;
    String header(const char* name) const;
    String hostHeader() const;
    String plainBody() const;

    // Response
    void send(int code, const char* content_type = "", const char* content = "");
    void send(int code, const char* content_type, const String& content);
    void sendHeader(const char* name, const char* value, bool first = false);
    void setContentLength(size_t len);

    uint16_t port() const { return port_; }

    // HTTP method constants
    static const int HTTP_GET    = 1;
    static const int HTTP_POST   = 2;
    static const int HTTP_PUT    = 3;
    static const int HTTP_DELETE = 4;
    static const int HTTP_PATCH  = 5;
    static const int HTTP_OPTIONS = 6;

private:
    struct Route {
        std::string uri;
        int method; // 0 = any
        THandlerFunction handler;
    };

    struct Param {
        std::string name;
        std::string value;
    };

    struct Header {
        std::string name;
        std::string value;
    };

    uint16_t port_;
    int server_fd_ = -1;
    bool running_ = false;

    std::vector<Route> routes_;
    THandlerFunction not_found_handler_;

    // Current request state
    std::string current_uri_;
    int current_method_ = 0;
    std::vector<Param> current_params_;
    std::vector<Header> current_headers_;
    std::string current_body_;
    int client_fd_ = -1;

    // Response headers to prepend
    std::vector<Header> response_headers_;
    size_t content_length_ = 0;

    void processClient(int fd);
    int parseMethod(const char* m);
    void parseQueryString(const std::string& qs);
    void parseHeaders(const std::string& raw);
};
