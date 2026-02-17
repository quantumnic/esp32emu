#include "Arduino.h"
#include "WiFi.h"
#include "WebServer.h"
#include <cassert>
#include <cstdio>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

static std::string http_get(uint16_t port, const char* path) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(fd);
        return "";
    }

    std::string req = std::string("GET ") + path + " HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
    send(fd, req.c_str(), req.size(), 0);

    std::string response;
    char buf[4096];
    while (true) {
        int n = recv(fd, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;
        buf[n] = '\0';
        response += buf;
    }
    close(fd);
    return response;
}

int main() {
    uint16_t port = 18732; // unlikely to conflict
    WebServer server(port);

    server.on("/", [&server]() {
        server.send(200, "text/plain", "Hello from esp32emu!");
    });

    server.on("/json", [&server]() {
        server.send(200, "application/json", "{\"status\":\"ok\"}");
    });

    server.on("/args", [&server]() {
        String val = server.arg("name");
        server.send(200, "text/plain", (String("name=") + val).c_str());
    });

    server.begin();

    // Handle a request in a thread
    auto handle_one = [&]() {
        for (int i = 0; i < 50; i++) {
            server.handleClient();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    };

    // Test 1: GET /
    {
        std::thread t(handle_one);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        auto resp = http_get(port, "/");
        t.join();
        assert(resp.find("200 OK") != std::string::npos);
        assert(resp.find("Hello from esp32emu!") != std::string::npos);
    }

    // Test 2: GET /json
    {
        std::thread t(handle_one);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        auto resp = http_get(port, "/json");
        t.join();
        assert(resp.find("application/json") != std::string::npos);
        assert(resp.find("{\"status\":\"ok\"}") != std::string::npos);
    }

    // Test 3: GET /args?name=test
    {
        std::thread t(handle_one);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        auto resp = http_get(port, "/args?name=test");
        t.join();
        assert(resp.find("name=test") != std::string::npos);
    }

    // Test 4: 404
    {
        std::thread t(handle_one);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        auto resp = http_get(port, "/nonexistent");
        t.join();
        assert(resp.find("404") != std::string::npos);
    }

    server.close();

    printf("test_webserver: all assertions passed\n");
    return 0;
}
