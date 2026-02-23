#include "ESPAsyncWebServer.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    AsyncWebServer server(80);

    std::string lastBody;

    server.on("/hello", HTTP_GET, [&](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Hello World");
    });

    server.on("/json", HTTP_GET, [&](AsyncWebServerRequest *request) {
        request->send(200, "application/json", "{\"status\":\"ok\"}");
    });

    server.on("/echo", HTTP_POST, [&](AsyncWebServerRequest *request) {
        lastBody = request->body();
        request->send(200, "text/plain", lastBody);
    });

    server.begin();

    // Simulate GET /hello
    {
        AsyncWebServerRequest req;
        req.setMethod("GET");
        req.setUrl("/hello");
        bool matched = server.simulateRequest(&req);
        assert(matched);
        assert(req.responded());
        assert(req.lastResponse().code() == 200);
        assert(req.lastResponse().body() == "Hello World");
    }

    // Simulate GET /json
    {
        AsyncWebServerRequest req;
        req.setMethod("GET");
        req.setUrl("/json");
        server.simulateRequest(&req);
        assert(req.responded());
        assert(req.lastResponse().body() == "{\"status\":\"ok\"}");
    }

    // Simulate POST /echo
    {
        AsyncWebServerRequest req;
        req.setMethod("POST");
        req.setUrl("/echo");
        req.setBody("ping");
        server.simulateRequest(&req);
        assert(req.responded());
        assert(lastBody == "ping");
    }

    // Query params
    {
        AsyncWebServerRequest req;
        req.setMethod("GET");
        req.setUrl("/hello");
        req.setParam("name", "esp32");
        server.simulateRequest(&req);
        assert(req.hasParam("name"));
        assert(req.getParam("name") == "esp32");
    }

    // Not found
    {
        AsyncWebServerRequest req;
        req.setMethod("GET");
        req.setUrl("/missing");
        bool matched = server.simulateRequest(&req);
        assert(!matched);
    }

    printf("test_async_webserver: all assertions passed\n");
    return 0;
}
