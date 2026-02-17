// ESP32 REST API Server — CRUD Todo List
// esp32emu run examples/rest_api/rest_api.ino
// Then: curl http://127.0.0.1:8080/api/todos
#include "Arduino.h"
#include "WiFi.h"
#include "WebServer.h"

WebServer server(8080);

struct Todo {
    int id;
    String title;
    bool done;
};

Todo todos[50];
int todoCount = 0;
int nextId = 1;

void sendJSON(int code, const String& json) {
    server.sendHeader("Content-Type", "application/json");
    server.send(code, "application/json", json);
}

void handleGetTodos() {
    String json = "[";
    for (int i = 0; i < todoCount; i++) {
        if (i > 0) json += ",";
        json += "{\"id\":" + String(todos[i].id);
        json += ",\"title\":\"" + todos[i].title + "\"";
        json += ",\"done\":" + String(todos[i].done ? "true" : "false") + "}";
    }
    json += "]";
    sendJSON(200, json);
    Serial.printf("\033[32m[GET] /api/todos → %d items\033[0m\n", todoCount);
}

void handlePostTodo() {
    if (todoCount >= 50) {
        sendJSON(400, "{\"error\":\"Todo list full\"}");
        return;
    }
    String body = server.plainBody();
    // Simple JSON parse for "title"
    int ti = body.indexOf("\"title\"");
    if (ti < 0) {
        sendJSON(400, "{\"error\":\"Missing title\"}");
        return;
    }
    int q1 = body.indexOf("\"", ti + 8);
    int q2 = body.indexOf("\"", q1 + 1);
    String title = body.substring(q1 + 1, q2);

    todos[todoCount] = {nextId++, title, false};
    todoCount++;

    String json = "{\"id\":" + String(todos[todoCount-1].id);
    json += ",\"title\":\"" + title + "\",\"done\":false}";
    sendJSON(201, json);
    Serial.printf("\033[32m[POST] Created: %s (id=%d)\033[0m\n", title.c_str(), todos[todoCount-1].id);
}

void handleDeleteTodo() {
    String uri = server.uri();
    // Parse id from /api/todos/N
    int lastSlash = uri.lastIndexOf('/');
    int id = uri.substring(lastSlash + 1).toInt();

    for (int i = 0; i < todoCount; i++) {
        if (todos[i].id == id) {
            for (int j = i; j < todoCount - 1; j++) todos[j] = todos[j+1];
            todoCount--;
            sendJSON(200, "{\"deleted\":" + String(id) + "}");
            Serial.printf("\033[33m[DELETE] Removed id=%d\033[0m\n", id);
            return;
        }
    }
    sendJSON(404, "{\"error\":\"Not found\"}");
}

void setup() {
    Serial.begin(115200);
    WiFi.begin("emu_network");

    server.on("/api/todos", WebServer::HTTP_GET, handleGetTodos);
    server.on("/api/todos", WebServer::HTTP_POST, handlePostTodo);
    // Note: DELETE routes need pattern matching; simplified here
    server.on("/api/todos/1", WebServer::HTTP_DELETE, handleDeleteTodo);
    server.on("/api/todos/2", WebServer::HTTP_DELETE, handleDeleteTodo);
    server.on("/api/todos/3", WebServer::HTTP_DELETE, handleDeleteTodo);

    server.on("/", []() {
        server.send(200, "text/html",
            "<h1>📝 Todo API</h1>"
            "<p>GET /api/todos</p>"
            "<p>POST /api/todos {\"title\":\"...\"}</p>"
            "<p>DELETE /api/todos/:id</p>");
    });

    server.begin();
    Serial.println("📝 Todo REST API running on http://127.0.0.1:8080");
    Serial.println("Try: curl http://127.0.0.1:8080/api/todos");
    Serial.println("     curl -X POST -d '{\"title\":\"Buy milk\"}' http://127.0.0.1:8080/api/todos");
}

void loop() {
    server.handleClient();
}
