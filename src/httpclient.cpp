#include "HTTPClient.h"
#include <cstdio>
#include <sstream>
#include <cstring>

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
        port_ = (uint16_t)std::stoi(host_.substr(colon + 1));
        host_ = host_.substr(0, colon);
    }

    return true;
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

    client.write((const uint8_t*)req.c_str(), req.size());

    // Read response
    std::string response;
    char buf[1024];
    while (true) {
        int n = client.read((uint8_t*)buf, sizeof(buf) - 1);
        if (n <= 0) break;
        buf[n] = '\0';
        response += buf;
    }

    client.stop();

    // Parse status code
    int status_code = -1;
    auto space = response.find(' ');
    if (space != std::string::npos) {
        status_code = std::stoi(response.substr(space + 1, 3));
    }

    // Extract body
    auto body_start = response.find("\r\n\r\n");
    if (body_start != std::string::npos) {
        response_body_ = response.substr(body_start + 4);
    }

    return status_code;
}

String HTTPClient::errorToString(int code) {
    if (code == -1) return String("Connection failed");
    return String(("HTTP " + std::to_string(code)).c_str());
}
