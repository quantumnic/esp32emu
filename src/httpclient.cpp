#include "HTTPClient.h"
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <sstream>
#include <cstring>
#include <algorithm>

static std::string to_lower_copy(const std::string& s) {
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c) { return (char)tolower(c); });
    return out;
}

static bool response_is_chunked(const std::string& head) {
    size_t pos = 0;
    while (pos < head.size()) {
        size_t eol = head.find('\n', pos);
        if (eol == std::string::npos) eol = head.size();
        std::string line = head.substr(pos, eol - pos);
        if (!line.empty() && line.back() == '\r') line.pop_back();
        auto colon = line.find(':');
        if (colon != std::string::npos) {
            std::string name = to_lower_copy(line.substr(0, colon));
            std::string value = to_lower_copy(line.substr(colon + 1));
            if (name == "transfer-encoding" &&
                value.find("chunked") != std::string::npos)
                return true;
        }
        pos = eol + 1;
    }
    return false;
}

static std::string decode_chunked_body(const std::string& in) {
    std::string out;
    size_t pos = 0;
    while (pos < in.size()) {
        size_t eol = in.find("\r\n", pos);
        if (eol == std::string::npos) break;
        char* end = nullptr;
        unsigned long sz = strtoul(in.c_str() + pos, &end, 16);
        if (end == in.c_str() + pos || sz == 0) break;
        pos = eol + 2;
        if (sz > in.size() - pos) {
            out.append(in, pos, in.size() - pos);
            break;
        }
        out.append(in, pos, sz);
        pos += sz + 2;
    }
    return out;
}

bool HTTPClient::begin(const char* url) {
    headers_.clear();
    response_body_.clear();

    std::string u(url);

    // Parse scheme
    bool is_https = false;
    if (u.rfind("https://", 0) == 0) { is_https = true; u = u.substr(8); }
    else if (u.rfind("http://", 0) == 0) { u = u.substr(7); }
    else return false;

    port_ = is_https ? 443 : 80;

    // Split host and path
    auto slash = u.find('/');
    if (slash != std::string::npos) {
        host_ = u.substr(0, slash);
        path_ = u.substr(slash);
    } else {
        host_ = u;
        path_ = "/";
    }

    // Check for port in host
    auto colon = host_.find(':');
    if (colon != std::string::npos) {
        char* end = nullptr;
        long p = strtol(host_.c_str() + colon + 1, &end, 10);
        if (end == host_.c_str() + colon + 1 || *end != '\0' || p <= 0 || p > 65535)
            return false;
        port_ = (uint16_t)p;
        host_ = host_.substr(0, colon);
    }

    return !host_.empty();
}

void HTTPClient::end() {
    response_body_.clear();
    headers_.clear();
}

void HTTPClient::addHeader(const char* name, const char* value) {
    headers_[name] = value;
}

int HTTPClient::GET() { return sendRequest("GET"); }
int HTTPClient::POST(const char* payload) { return sendRequest("POST", payload); }
int HTTPClient::PUT(const char* payload) { return sendRequest("PUT", payload); }

int HTTPClient::sendRequest(const char* method, const char* payload) {
    WiFiClient client;
    client.setTimeout(timeout_ms_);
    if (!client.connect(host_.c_str(), port_)) return -1;

    // Build request
    std::string req;
    req += std::string(method) + " " + path_ + " HTTP/1.1\r\n";
    req += "Host: " + host_ + "\r\n";

    for (auto& h : headers_)
        req += h.first + ": " + h.second + "\r\n";

    if (payload) {
        req += "Content-Length: " + std::to_string(strlen(payload)) + "\r\n";
        if (headers_.find("Content-Type") == headers_.end())
            req += "Content-Type: application/x-www-form-urlencoded\r\n";
    }

    req += "Connection: close\r\n\r\n";
    if (payload) req += payload;

    size_t sent = client.write((const uint8_t*)req.c_str(), req.size());
    if (sent != req.size()) {
        client.stop();
        return -1;
    }

    // Read response
    std::string response;
    char buf[4096];
    while (true) {
        int n = client.read((uint8_t*)buf, sizeof(buf));
        if (n <= 0) break;
        response.append(buf, (size_t)n);
    }

    client.stop();

    // Parse status code
    int status_code = -1;
    auto version = response.find("HTTP/");
    if (version == 0) {
        auto space = response.find(' ');
        if (space != std::string::npos && space + 1 < response.size()) {
            char* end = nullptr;
            long v = strtol(response.c_str() + space + 1, &end, 10);
            if (end != response.c_str() + space + 1 && v >= 100 && v <= 599)
                status_code = (int)v;
        }
    }

    // Extract body
    auto body_start = response.find("\r\n\r\n");
    std::string raw_head = response.substr(0, body_start == std::string::npos ? response.size() : body_start);
    if (body_start != std::string::npos) {
        std::string raw_body = response.substr(body_start + 4);
        response_body_ =
            response_is_chunked(raw_head) ? decode_chunked_body(raw_body) : raw_body;
    } else {
        response_body_.clear();
    }

    return status_code;
}

String HTTPClient::errorToString(int code) {
    if (code == -1) return String("Connection failed");
    return String(("HTTP " + std::to_string(code)).c_str());
}
