# esp32emu

**Lightweight ESP32/Arduino emulator for host-side testing — real network sockets, no hardware needed.**

Run your ESP32 sketches on macOS or Linux. The HTTP server listens on localhost with real POSIX sockets — test with `curl`, your browser, or integration tests. No QEMU, no Wokwi, no hardware.

## Quick Start

```bash
git clone https://github.com/redbasecap-buiss/esp32emu.git
cd esp32emu

# Run the self-tests
make test

# Run a sketch
./esp32emu run examples/webserver_demo.cpp
# → Open http://127.0.0.1:8080 in your browser

# Build a sketch to a binary
./esp32emu build examples/blink.cpp -o blink
./blink
```

## Use as a Library

Include esp32emu headers in your project and link against `libesp32emu.a`:

```bash
make lib
c++ -std=c++17 -Iinclude my_sketch.cpp src/esp32emu_main.cpp -Lbuild -lesp32emu -o my_sketch
./my_sketch
```

### CMake

```cmake
add_subdirectory(esp32emu)
target_link_libraries(my_project esp32emu esp32emu_main)
```

### PlatformIO

```ini
[env:native]
platform = native
build_flags = -std=c++17 -Iesp32emu/include
src_filter = +<*> +<../esp32emu/src/*>
```

## API Coverage

| Header | Status | Notes |
|---|---|---|
| `Arduino.h` | ✅ Full | millis, delay, pinMode, digital/analogRead/Write, String, Serial |
| `ESP.h` | ✅ Full | getFreeHeap, restart, getChipId, getChipModel |
| `WiFi.h` | ✅ Full | begin → auto-connect, localIP → 127.0.0.1, softAP, RSSI |
| `WebServer.h` | ✅ Full | **Real POSIX sockets** — GET/POST/PUT/DELETE, query params, headers |
| `WiFiClient.h` | ✅ Full | **Real TCP sockets** — connect, read, write |
| `HTTPClient.h` | ✅ Full | **Real HTTP requests** — GET/POST/PUT |
| `EEPROM.h` | ✅ Full | RAM-backed, struct put/get |
| `Wire.h` | ✅ Full | I2C mock with configurable device callbacks |
| `SPI.h` | ✅ Mock | Echo-mode transfers |
| `ESPmDNS.h` | ✅ Mock | Register only, no broadcast |
| `WiFiClientSecure.h` | ✅ Mock | Stub for TLS (uses plain TCP) |

## GPIO Simulation

All 40 GPIO pins are simulated with full state tracking:

```cpp
pinMode(LED_BUILTIN, OUTPUT);
digitalWrite(LED_BUILTIN, HIGH);
int state = digitalRead(LED_BUILTIN); // → 1

// Interrupt support
attachInterrupt(4, myISR, RISING);
```

Set analog inputs programmatically for testing:

```cpp
esp32emu::GPIOController::instance().setAnalogInput(A0, 2048);
int val = analogRead(A0); // → 2048
```

## WebServer — Real HTTP on localhost

The emulated `WebServer` binds to an actual port. Test your ESP32 HTTP API with real tools:

```cpp
WebServer server(8080);

void setup() {
    server.on("/api/data", []() {
        server.send(200, "application/json", "{\"temp\":22.5}");
    });
    server.begin();
}

void loop() {
    server.handleClient();
}
```

```bash
$ curl http://127.0.0.1:8080/api/data
{"temp":22.5}
```

## I2C Mock Devices

Register mock I2C devices with custom response logic:

```cpp
Wire.addDevice(0x48, [](const std::vector<uint8_t>& tx, uint8_t qty) {
    return std::vector<uint8_t>{0x00, 0xC8}; // 25°C
});
```

## vs. Alternatives

| | esp32emu | Wokwi | QEMU |
|---|---|---|---|
| **Setup** | `git clone` + `make` | Browser / cloud | Complex config |
| **Network** | Real POSIX sockets | Simulated | Bridge config |
| **Speed** | Native (instant) | JS overhead | Emulation overhead |
| **CI-friendly** | ✅ `make test` | ❌ Cloud-based | ⚠️ Heavy |
| **Peripherals** | Mock callbacks | Visual UI | Partial |
| **Binary compat** | Source-level | Binary | Binary |

esp32emu is **source-level compatible** — your code compiles against these headers instead of the real SDK. It's designed for fast iteration and CI, not cycle-accurate emulation.

## Used By

- [**mcpd**](https://github.com/redbasecap-buiss/mcpd) — MCP server for ESP32

## Requirements

- C++17 compiler (clang++ or g++)
- macOS (arm64/x86_64) or Linux (x86_64/arm64)
- Make or CMake ≥ 3.14

## License

MIT — see [LICENSE](LICENSE).
