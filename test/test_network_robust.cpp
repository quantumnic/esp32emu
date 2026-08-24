#include "Arduino.h"
#include "WiFi.h"
#include "WebServer.h"
#include "HTTPClient.h"
#include "esp32emu_socket.h"
#include <cassert>
#include <cstdio>
#include <thread>
#include <chrono>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <unistd.h>
#include <cstring>
#include <string>

static std::atomic<bool> pumping{true};

static int connect_to(uint16_t port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        close(fd);
        return -1;
    }
    return fd;
}

static std::string http_get(uint16_t port, const char* path) {
    int fd = connect_to(port);
    if (fd < 0) return "";
    std::string req = std::string("GET ") + path + " HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n";
    send(fd, req.c_str(), req.size(), 0);
    std::string response;
    char buf[4096];
    while (true) {
        int n = (int)recv(fd, buf, sizeof(buf), 0);
        if (n <= 0) break;
        response.append(buf, (size_t)n);
    }
    close(fd);
    return response;
}

static void pump_for(std::function<bool()> done, int max_ms) {
    // handled externally by background thread; just wait for condition
    int waited = 0;
    while (!done() && waited < max_ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        waited += 5;
    }
}

int main() {
    // ── Unit: url_decode ───────────────────────────────────────────
    {
        using esp32emu::net::url_decode;
        assert(url_decode("hello%20world") == "hello world");
        assert(url_decode("a+b") == "a b");
        assert(url_decode("plus%2Bplus") == "plus+plus");
        assert(url_decode("bad%zz") == "bad%zz");
        assert(url_decode("trailing%") == "trailing%");
        assert(url_decode("trailing%2") == "trailing%2");
        assert(url_decode("no-encoding") == "no-encoding");
        printf("url_decode: OK\n");
    }

    uint16_t port = 18841;
    WebServer server(port);

    static bool g_body_ok = false;

    server.on("/", [&server]() { server.send(200, "text/plain", "root-ok"); });

    server.on("/args", [&server]() {
        String v = server.arg("v");
        String w = server.arg("w");
        server.send(200, "text/plain", (String("v=") + v + ";w=" + w).c_str());
    });

    server.on("/form", [&server]() {
        String color = server.arg("color");
        String n = server.arg("n");
        server.send(200, "text/plain", (String("color=") + color + ";n=" + n).c_str());
    });

    server.on("/upload", [&server]() {
        std::string body(server.plainBody().c_str(), strlen(server.plainBody().c_str()));
        g_body_ok = (body.size() == 60000 && body[0] == 'A' &&
                     body.back() == 'Z');
        server.send(200, "text/plain", ("len=" + std::to_string(body.size())).c_str());
    });

    static std::string big_payload(6 * 1024 * 1024, 'x');
    server.on("/big", [&server]() { server.send(200, "text/plain", big_payload.c_str()); });

    server.begin();

    std::thread pump([&server]() {
        while (pumping.load()) {
            server.handleClient();
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        }
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    // ── Test 1: split large POST arrives intact ────────────────────
    {
        g_body_ok = false;
        int fd = connect_to(port);
        assert(fd >= 0);
        std::string head = "POST /upload HTTP/1.1\r\nHost: t\r\nContent-Type: text/plain\r\nContent-Length: 60000\r\nConnection: close\r\n\r\n";
        std::string body(60000, '.');
        body[0] = 'A';
        body[59999] = 'Z';
        send(fd, head.c_str(), head.size(), 0);
        send(fd, body.data(), 1000, 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
        send(fd, body.data() + 1000, body.size() - 1000, 0);
        std::string resp;
        char buf[2048];
        while (true) {
            int n = (int)recv(fd, buf, sizeof(buf), 0);
            if (n <= 0) break;
            resp.append(buf, (size_t)n);
        }
        close(fd);
        pump_for([&]() { return resp.find("len=60000") != std::string::npos || g_body_ok; }, 5000);
        assert(resp.find("200 OK") != std::string::npos);
        assert(g_body_ok);
        printf("split large POST: OK\n");
    }

    // ── Test 2: URL-decoded query args ─────────────────────────────
    {
        auto resp = http_get(port, "/args?v=hello%20world&w=a%2Bb");
        assert(resp.find("200 OK") != std::string::npos);
        assert(resp.find("v=hello world") != std::string::npos);
        assert(resp.find("w=a+b") != std::string::npos);
        printf("query arg url-decoding: OK\n");
    }

    // ── Test 3: form-encoded POST body decoding ────────────────────
    {
        int fd = connect_to(port);
        assert(fd >= 0);
        std::string body = "color=blue%20car&x=1&y=a+b";
        std::string req = "POST /form HTTP/1.1\r\nHost: t\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: " +
                          std::to_string(body.size()) + "\r\nConnection: close\r\n\r\n" + body;
        send(fd, req.c_str(), req.size(), 0);
        std::string resp;
        char buf[2048];
        while (true) {
            int n = (int)recv(fd, buf, sizeof(buf), 0);
            if (n <= 0) break;
            resp.append(buf, (size_t)n);
        }
        close(fd);
        assert(resp.find("color=blue car") != std::string::npos);
        assert(resp.find(";n=") != std::string::npos);
        printf("form body url-decoding: OK\n");
    }

    // ── Test 4: aborted client must not kill the server ────────────
    {
        for (int i = 0; i < 3; i++) {
            int fd = connect_to(port);
            assert(fd >= 0);
            std::string req = "GET /big HTTP/1.1\r\nHost: t\r\nConnection: close\r\n\r\n";
            send(fd, req.c_str(), req.size(), 0);
            struct pollfd pfd{fd, POLLIN, 0};
            poll(&pfd, 1, 3000);
            char b[64];
            recv(fd, b, sizeof(b), MSG_DONTWAIT);
            struct linger lg{};
            lg.l_onoff = 1;
            lg.l_linger = 0;
            setsockopt(fd, SOL_SOCKET, SO_LINGER, &lg, sizeof(lg));
            close(fd);
            std::this_thread::sleep_for(std::chrono::milliseconds(150));
        }
        auto resp = http_get(port, "/");
        assert(resp.find("root-ok") != std::string::npos);
        printf("abort mid-response survival: OK\n");
    }

    // ── Test 5: HTTPClient round-trip against local WebServer ──────
    {
        HTTPClient hc;
        assert(hc.begin("http://127.0.0.1:18841/"));
        int rc = hc.GET();
        assert(rc == 200);
        assert(hc.getSize() == 7);
        assert(std::string(hc.getString().c_str()) == "root-ok");
        hc.end();
        printf("HTTPClient round-trip: OK\n");
    }

    pumping.store(false);
    pump.join();
    server.close();

    // ── Test 6: chunked transfer-encoding decoding ─────────────────
    {
        uint16_t cport = 18842;
        int lfd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = htons(cport);
        assert(bind(lfd, (struct sockaddr*)&addr, sizeof(addr)) == 0);
        assert(listen(lfd, 1) == 0);

        std::thread serve([&]() {
            int cfd = accept(lfd, nullptr, nullptr);
            std::string req;
            char b[1024];
            while (req.find("\r\n\r\n") == std::string::npos) {
                ssize_t n = recv(cfd, b, sizeof(b), 0);
                if (n <= 0) break;
                req.append(b, (size_t)n);
            }
            const char* resp =
                "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n"
                "6\r\nHello \r\nD\r\nChunked World\r\n0\r\n\r\n";
            send(cfd, resp, strlen(resp), 0);
            close(cfd);
        });

        HTTPClient hc;
        hc.setTimeout(2000);
        assert(hc.begin("http://127.0.0.1:18842/x"));
        int rc = hc.GET();
        serve.join();
        close(lfd);
        assert(rc == 200);
        assert(std::string(hc.getString().c_str()) == "Hello Chunked World");
        hc.end();
        printf("chunked response decoding: OK\n");
    }

    // ── Test 7: garbage response must not crash or throw ───────────
    {
        uint16_t gport = 18843;
        int lfd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = htons(gport);
        assert(bind(lfd, (struct sockaddr*)&addr, sizeof(addr)) == 0);
        assert(listen(lfd, 1) == 0);

        std::thread serve([&]() {
            int cfd = accept(lfd, nullptr, nullptr);
            const char* resp = "GARBAGE NOT HTTP AT ALL\r\n\r\n";
            send(cfd, resp, strlen(resp), 0);
            close(cfd);
        });

        HTTPClient hc;
        hc.setTimeout(2000);
        assert(hc.begin("http://127.0.0.1:18843/x"));
        int rc = hc.GET();
        serve.join();
        close(lfd);
        assert(rc < 0);
        assert(std::string(hc.errorToString(-1).c_str()) == "Connection failed");
        printf("garbage response safety: OK\n");
    }

    // ── Test 8: timeout honored against silent server ──────────────
    {
        uint16_t tport = 18844;
        int lfd = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = htons(tport);
        assert(bind(lfd, (struct sockaddr*)&addr, sizeof(addr)) == 0);
        assert(listen(lfd, 1) == 0);

        HTTPClient hc;
        hc.setTimeout(400);
        assert(hc.begin("http://127.0.0.1:18844/slow"));
        auto start = std::chrono::steady_clock::now();
        int rc = hc.GET();
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                              std::chrono::steady_clock::now() - start)
                              .count();
        close(lfd);
        assert(rc < 0);
        assert(elapsed_ms < 3000);
        printf("timeout honored (%lld ms): OK\n", (long long)elapsed_ms);
    }

    printf("test_network_robust: all assertions passed\n");
    return 0;
}
