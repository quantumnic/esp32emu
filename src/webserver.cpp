#include "WebServer.h"
#include "esp32emu_socket.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
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

    // Accepted sockets inherit O_NONBLOCK from the listener on some
    // platforms (macOS); restore blocking mode so SO_RCVTIMEO applies.
    int fd_flags = fcntl(fd, F_GETFL, 0);
    if (fd_flags >= 0) fcntl(fd, F_SETFL, fd_flags & ~O_NONBLOCK);

    // Set a read timeout
    struct timeval tv{};
    tv.tv_sec = 1;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    esp32emu::net::harden_socket(fd);

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
            current_params_.push_back({esp32emu::net::url_decode(pair.substr(0, eq)),
                                       esp32emu::net::url_decode(pair.substr(eq + 1))});
        } else {
            current_params_.push_back({esp32emu::net::url_decode(pair), ""});
        }
    }
}

static size_t parse_content_length(const std::string& headers) {
    size_t pos = 0;
    bool found = false;
    size_t value = 0;
    while (pos < headers.size()) {
        size_t eol = headers.find('\n', pos);
        if (eol == std::string::npos) eol = headers.size();
        std::string line = headers.substr(pos, eol - pos);
        if (!line.empty() && line.back() == '\r') line.pop_back();
        auto colon = line.find(':');
        if (colon != std::string::npos) {
            std::string name = line.substr(0, colon);
            std::transform(name.begin(), name.end(), name.begin(),
                           [](unsigned char c) { return (char)tolower(c); });
            if (name == "content-length") {
                char* end = nullptr;
                long v = strtol(line.c_str() + colon + 1, &end, 10);
                if (end != line.c_str() + colon + 1 && v >= 0) {
                    value = (size_t)v;
                    found = true;
                }
            }
        }
        pos = eol + 1;
    }
    return found ? value : (size_t)-1;
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

static const size_t kMaxHeaderBytes = 64 * 1024;
static const size_t kMaxBodyBytes = 16 * 1024 * 1024;

void WebServer::processClient(int fd) {
    std::string req;
    char buf[8192];
    size_t header_end = std::string::npos;
    size_t content_length = (size_t)-1;

    while (true) {
        if (header_end != std::string::npos) {
            size_t body_have = req.size() - (header_end + 4);
            if (content_length != (size_t)-1 && body_have >= content_length) break;
        } else if (req.size() > kMaxHeaderBytes) {
            break;
        }
        ssize_t n = ::recv(fd, buf, sizeof(buf), 0);
        if (n <= 0) break;
        req.append(buf, (size_t)n);
        if (header_end == std::string::npos) {
            header_end = req.find("\r\n\r\n");
            if (header_end != std::string::npos)
                content_length = parse_content_length(req.substr(0, header_end));
        }
    }

    if (req.empty()) return;

    client_fd_ = fd;
    current_params_.clear();
    current_headers_.clear();
    response_headers_.clear();
    current_body_.clear();

    // Parse request line
    size_t line_end = req.find("\r\n");
    std::string request_line =
        req.substr(0, line_end == std::string::npos ? req.size() : line_end);
    if (request_line.size() >= 2048) {
        client_fd_ = -1;
        return;
    }
    char method_str[16], path_buf[1024], proto[16];
    if (sscanf(request_line.c_str(), "%15s %1023s %15s", method_str, path_buf, proto) < 2) {
        client_fd_ = -1;
        return;
    }

    current_method_ = parseMethod(method_str);

    // Split path and query string
    std::string full_path(path_buf);
    auto qmark = full_path.find('?');
    if (qmark != std::string::npos) {
        current_uri_ = esp32emu::net::url_decode(full_path.substr(0, qmark));
        parseQueryString(full_path.substr(qmark + 1));
    } else {
        current_uri_ = esp32emu::net::url_decode(full_path);
    }

    // Parse headers
    if (line_end != std::string::npos && header_end != std::string::npos &&
        header_end > line_end + 2) {
        parseHeaders(req.substr(line_end + 2, header_end - (line_end + 2)));
    } else if (line_end != std::string::npos) {
        parseHeaders(req.substr(line_end + 2));
    }

    // Extract body up to Content-Length (or all buffered data if absent)
    if (header_end != std::string::npos && req.size() > header_end + 4) {
        size_t remaining = req.size() - (header_end + 4);
        size_t avail = (content_length != (size_t)-1) ? std::min(content_length, remaining)
                                                      : remaining;
        avail = std::min(avail, kMaxBodyBytes);
        current_body_ = req.substr(header_end + 4, avail);
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

    size_t off = 0;
    while (off < resp.size()) {
        ssize_t n = esp32emu::net::safe_send(client_fd_, resp.data() + off, resp.size() - off);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) break;
        off += (size_t)n;
    }
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
