# Architecture

This document explains how esp32emu works internally.

## Overview

esp32emu is a **functional emulator** — it replicates the Arduino/ESP32 API surface so your sketches can run on a host machine (macOS/Linux) without hardware. It is **not** a cycle-accurate hardware simulator.

```
┌─────────────────────────────────────────────────────────┐
│                    Your Sketch                           │
│               (setup() + loop())                         │
├─────────────────────────────────────────────────────────┤
│              Arduino API Layer                           │
│  Arduino.h │ WiFi.h │ WebServer.h │ Wire.h │ SPI.h │…  │
├─────────────────────────────────────────────────────────┤
│           Emulation Backend                              │
│  GPIO State │ POSIX Sockets │ I2C Bus │ Memory Store    │
├─────────────────────────────────────────────────────────┤
│           Board Configuration                            │
│  Pin count │ Chip info │ Feature flags (WiFi/BT)        │
├─────────────────────────────────────────────────────────┤
│           Terminal UI                                    │
│  Board diagram │ Pin states │ LED/Servo/LCD visuals     │
└─────────────────────────────────────────────────────────┘
```

## Key Design Decisions

### Header-Only Where Possible

Most mocks are implemented entirely in header files. This keeps the build simple and allows sketches to `#include` what they need without complex linking.

### Real POSIX Sockets

Unlike most Arduino emulators that stub out networking, esp32emu uses actual TCP sockets. `WebServer` binds to a real port, `HTTPClient` makes real HTTP requests. This means you can test your REST APIs with `curl` or a browser.

### Board Abstraction

The `esp32emu_board.h` module defines board configurations:

```cpp
struct BoardConfig {
    const char* name;
    const char* chip;
    int digital_pins;
    int analog_pins;
    int ram_kb;
    int flash_kb;
    bool has_wifi;
    bool has_bluetooth;
    int led_builtin;
};
```

When you select `--board uno`, the emulator loads the Uno configuration and enforces its constraints (pin count, no WiFi, etc.).

### Terminal UI

The terminal renderer (`esp32emu_terminal.h`) uses ANSI escape codes:
- **Board diagrams** — ASCII art drawn at startup based on the active board
- **GPIO states** — Updated inline as `digitalWrite`/`digitalRead` are called
- **Peripheral visuals** — LED brightness bars, servo angle indicators, LCD content boxes
- **Colored serial** — `[INFO]`/`[WARN]`/`[ERROR]` prefixes get colored automatically

## Module Map

| File | Purpose |
|------|---------|
| `include/Arduino.h` | Core: `pinMode`, `digitalWrite`, `delay`, `Serial`, `millis` |
| `include/esp32emu_gpio.h` | GPIO state machine with per-pin mode/value tracking |
| `include/esp32emu_board.h` | Board definitions and pin validation |
| `include/esp32emu_terminal.h` | ANSI terminal rendering (boards, pins, peripherals) |
| `include/esp32emu_serial.h` | Serial mock with colored output |
| `include/WiFi.h` | WiFi STA/AP simulation |
| `include/WebServer.h` | HTTP server using POSIX sockets |
| `src/webserver.cpp` | WebServer socket implementation |
| `src/httpclient.cpp` | HTTPClient socket implementation |
| `src/wificlient.cpp` | WiFiClient socket implementation |
| `src/esp32emu_main.cpp` | CLI entry point, sketch loader |
| `src/globals.cpp` | Singleton instances (`WiFi`, `EEPROM`, etc.) |

## Build System

The project uses a simple Makefile:

- `make` — Build the `esp32emu` binary
- `make lib` — Build `libesp32emu.a` static library
- `make test` — Build and run all test binaries
- `make clean` — Remove build artifacts

No CMake, no external dependencies. Just C++17 and POSIX.

## Networking Stack

```
WebServer.h                    HTTPClient.h
    │                              │
    ▼                              ▼
socket() + bind() + listen()   socket() + connect()
    │                              │
    ▼                              ▼
accept() → read/write         write() → read()
    │                              │
    └──────────── POSIX ───────────┘
```

The HTTP parser in `WebServer` is minimal but handles:
- GET/POST/PUT/DELETE methods
- URL routing with handler callbacks
- Query parameters
- Request body parsing
- Custom headers
