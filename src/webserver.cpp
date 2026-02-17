#include "WebServer.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstdio>
#include <sstream>
#include <algorithm>

WebServer::WebServer(uint16_t port) : port_(port) {}

WebServer::~WebServer() { close(); }

void WebServer::begin() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        fprintf(stderr, "[esp32emu] WebServer: socket() failed\n");
        return;
    }

    int opt = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);

    if (bind(server_fd_, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "[esp32emu] WebServer: bind() failed on port %d: %s\n",
                port_, strerror(errno));
        ::close(server_fd_);
        server_fd_ = -1;
        return;
    }

    listen(server_fd_, 5);

    // Non-blocking
    int flags = fcntl(server_fd_, F_GETFL, 0);
    fcntl(server_fd_, F_SETFL, flags | O_NONBLOCK);

    running_ = true;
    fprintf(stderr, "[esp32emu] WebServer: listening on http://127.0.0.1:%d\n", port_);
}

void WebServer::close() {
    if (server_fd_ >= 0) {
        ::close(server_fd_);
        server_fd_ = -1;
    }
    running_ = false;
}

void WebServer::on(const char* uri, THandlerFunction handler) {
    routes_.push_back({uri, 0, handler});
}

void WebServer::on(const char* uri, int method, THandlerFunction handler) {
    routes_.push_back({uri, method, handler});
}

void WebServer::onNotFound(THandlerFunction handler) {
    not_found_handler_ = handler;
}

void WebServer::handleClient() {
    if (server_fd_ < 0) return;

    struct sockaddr_in client_addr{};
    socklen_t len = sizeof(client_addr);
    int fd = accept(server_fd_, (struct sockaddr*)&client_addr, &len);
    if (fd < 0) return;

    // Set a read timeout
    struct timeval tv{};
    tv.tv_sec = 1;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    processClient(fd);
    ::close(fd);
}

int WebServer::parseMethod(const char* m) {
    if (strcmp(m, "GET") == 0) return HTTP_GET;
    if (strcmp(m, "POST") == 0) return HTTP_POST;
    if (strcmp(m, "PUT") == 0) return HTTP_PUT;
    if (strcmp(m, "DELETE") == 0) return HTTP_DELETE;
    if (strcmp(m, "PATCH") == 0) return HTTP_PATCH;
    if (strcmp(m, "OPTIONS") == 0) return HTTP_OPTIONS;
    return 0;
}

void WebServer::parseQueryString(const std::string& qs) {
    std::istringstream ss(qs);
    std::string pair;
    while (std::getline(ss, pair, '&')) {
        auto eq = pair.find('=');
        if (eq != std::string::npos) {
            current_params_.push_back({pair.substr(0, eq), pair.substr(eq + 1)});
        } else {
            current_params_.push_back({pair, ""});
        }
    }
}

void WebServer::parseHeaders(const std::string& raw) {
    std::istringstream ss(raw);
    std::string line;
    while (std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        auto colon = line.find(':');
        if (colon != std::string::npos) {
            std::string name = line.substr(0, colon);
            std::string val = line.substr(colon + 1);
            // trim leading space
            if (!val.empty() && val[0] == ' ') val = val.substr(1);
            current_headers_.push_back({name, val});
        }
    }
}

void WebServer::processClient(int fd) {
    char buf[8192];
    int n = recv(fd, buf, sizeof(buf) - 1, 0);
    if (n <= 0) return;
    buf[n] = '\0';

    client_fd_ = fd;
    current_params_.clear();
    current_headers_.clear();
    response_headers_.clear();
    current_body_.clear();

    // Parse request line
    char method_str[16], path_buf[1024], proto[16];
    if (sscanf(buf, "%15s %1023s %15s", method_str, path_buf, proto) != 3) {
        client_fd_ = -1;
        return;
    }

    current_method_ = parseMethod(method_str);

    // Split path and query string
    std::string full_path(path_buf);
    auto qmark = full_path.find('?');
    if (qmark != std::string::npos) {
        current_uri_ = full_path.substr(0, qmark);
        parseQueryString(full_path.substr(qmark + 1));
    } else {
        current_uri_ = full_path;
    }

    // Parse headers
    const char* header_start = strstr(buf, "\r\n");
    if (header_start) {
        header_start += 2;
        const char* body_start = strstr(header_start, "\r\n\r\n");
        if (body_start) {
            std::string hdr_block(header_start, body_start - header_start);
            parseHeaders(hdr_block);
            current_body_ = std::string(body_start + 4);
        } else {
            parseHeaders(std::string(header_start));
        }
    }

    // If POST body and content-type is form, parse params
    if (current_method_ == HTTP_POST && !current_body_.empty()) {
        std::string ct = header("Content-Type").c_str();
        if (ct.find("application/x-www-form-urlencoded") != std::string::npos) {
            parseQueryString(current_body_);
        }
    }

    // Find matching route
    bool found = false;
    for (auto& r : routes_) {
        if (r.uri == current_uri_ && (r.method == 0 || r.method == current_method_)) {
            r.handler();
            found = true;
            break;
        }
    }

    if (!found) {
        if (not_found_handler_) {
            not_found_handler_();
        } else {
            send(404, "text/plain", "Not Found");
        }
    }

    client_fd_ = -1;
}

String WebServer::uri() const { return String(current_uri_.c_str()); }
int WebServer::method() const { return current_method_; }

String WebServer::arg(const char* name) const {
    for (auto& p : current_params_)
        if (p.name == name) return String(p.value.c_str());
    return String("");
}

String WebServer::arg(int i) const {
    if (i >= 0 && i < (int)current_params_.size())
        return String(current_params_[i].value.c_str());
    return String("");
}

String WebServer::argName(int i) const {
    if (i >= 0 && i < (int)current_params_.size())
        return String(current_params_[i].name.c_str());
    return String("");
}

int WebServer::args() const { return (int)current_params_.size(); }

bool WebServer::hasArg(const char* name) const {
    for (auto& p : current_params_)
        if (p.name == name) return true;
    return false;
}

String WebServer::header(const char* name) const {
    for (auto& h : current_headers_)
        if (strcasecmp(h.name.c_str(), name) == 0)
            return String(h.value.c_str());
    return String("");
}

String WebServer::hostHeader() const { return header("Host"); }
String WebServer::plainBody() const { return String(current_body_.c_str()); }

void WebServer::send(int code, const char* content_type, const char* content) {
    if (client_fd_ < 0) return;

    const char* reason = "OK";
    if (code == 200) reason = "OK";
    else if (code == 201) reason = "Created";
    else if (code == 204) reason = "No Content";
    else if (code == 301) reason = "Moved Permanently";
    else if (code == 302) reason = "Found";
    else if (code == 400) reason = "Bad Request";
    else if (code == 401) reason = "Unauthorized";
    else if (code == 403) reason = "Forbidden";
    else if (code == 404) reason = "Not Found";
    else if (code == 500) reason = "Internal Server Error";

    std::string resp;
    resp += "HTTP/1.1 " + std::to_string(code) + " " + reason + "\r\n";

    for (auto& h : response_headers_)
        resp += h.name + ": " + h.value + "\r\n";

    if (content_type && strlen(content_type) > 0)
        resp += "Content-Type: " + std::string(content_type) + "\r\n";

    size_t clen = content ? strlen(content) : 0;
    resp += "Content-Length: " + std::to_string(clen) + "\r\n";
    resp += "Connection: close\r\n";
    resp += "\r\n";
    if (content && clen > 0) resp += content;

    ::send(client_fd_, resp.c_str(), resp.size(), 0);
    response_headers_.clear();
}

void WebServer::send(int code, const char* ct, const String& content) {
    send(code, ct, content.c_str());
}

void WebServer::sendHeader(const char* name, const char* value, bool first) {
    Header h{name, value};
    if (first)
        response_headers_.insert(response_headers_.begin(), h);
    else
        response_headers_.push_back(h);
}

void WebServer::setContentLength(size_t len) {
    content_length_ = len;
}
