#pragma once
#include "WiFiClient.h"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <queue>

#ifdef _WIN32
  #include <winsock2.h>
#else
  #include <unistd.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <fcntl.h>
  #include <errno.h>
#endif

// WiFiServer mock — real TCP listener on loopback
class WiFiServer {
public:
    WiFiServer(uint16_t port = 80) : port_(port) {}

    ~WiFiServer() { stop(); }

    void begin() {
        server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd_ < 0) {
            fprintf(stderr, "[esp32emu] WiFiServer: socket() failed\n");
            return;
        }

        int opt = 1;
        setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        struct sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr.sin_port = htons(port_);

        if (bind(server_fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            fprintf(stderr, "[esp32emu] WiFiServer: bind(:%d) failed: %s\n", port_, strerror(errno));
            close(server_fd_);
            server_fd_ = -1;
            return;
        }

        if (listen(server_fd_, 5) < 0) {
            fprintf(stderr, "[esp32emu] WiFiServer: listen() failed\n");
            close(server_fd_);
            server_fd_ = -1;
            return;
        }

        // Non-blocking so available() doesn't hang
        fcntl(server_fd_, F_SETFL, O_NONBLOCK);

        listening_ = true;
        fprintf(stderr, "[esp32emu] WiFiServer: listening on 127.0.0.1:%d\n", port_);
    }

    void begin(uint16_t port) {
        port_ = port;
        begin();
    }

    WiFiClient available() {
        if (server_fd_ < 0) return WiFiClient();

        struct sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        int client_fd = accept(server_fd_, (struct sockaddr*)&client_addr, &len);
        if (client_fd < 0) {
            return WiFiClient(); // No pending connection
        }
        // Set client socket to non-blocking for available() to work
        fcntl(client_fd, F_SETFL, O_NONBLOCK);
        return WiFiClient("127.0.0.1", 0, client_fd);
    }

    void stop() {
        if (server_fd_ >= 0) {
            close(server_fd_);
            server_fd_ = -1;
            listening_ = false;
            fprintf(stderr, "[esp32emu] WiFiServer: stopped\n");
        }
    }

    operator bool() const { return listening_; }

    uint16_t getPort() const { return port_; }

private:
    uint16_t port_;
    int server_fd_ = -1;
    bool listening_ = false;
};
