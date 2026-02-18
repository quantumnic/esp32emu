# Examples

Detailed walkthroughs of all included examples.

## Basics

### `blink/` — LED Blink

The classic Arduino starter. Toggles `LED_BUILTIN` on and off with a 1-second delay.

```cpp
void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
}
void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
}
```

```bash
./esp32emu run examples/blink/blink.ino
./esp32emu run --board uno examples/blink/blink.ino  # works on any board
```

### `button_led/` — Button Input → LED

Demonstrates `digitalRead` with a simulated button press. The emulator generates periodic HIGH/LOW transitions on the input pin.

```bash
./esp32emu run --board uno examples/button_led/button_led.ino
```

## Networking

### `rest_api/` — CRUD REST API

A full Todo REST API running on `localhost:8080`:

```bash
./esp32emu run examples/rest_api/rest_api.ino
# In another terminal:
curl http://localhost:8080/api/todos
curl -X POST -d '{"title":"Buy milk"}' http://localhost:8080/api/todos
```

### `webserver_demo.cpp` — WebServer Demo

Shows multiple routes, query parameters, and JSON responses.

```bash
./esp32emu run examples/webserver_demo.cpp
curl http://localhost:8080/
curl http://localhost:8080/api/status
```

### `mqtt_client/` — MQTT Client

Simulates MQTT publish/subscribe. No broker needed — the mock generates fake messages.

### `captive_portal/` — WiFi Captive Portal

Sets up an AP mode WiFi portal for configuration. The web UI is served on localhost.

### `ota_update/` — OTA Update

Simulates an over-the-air firmware update with a progress bar in the terminal.

## Sensors & Peripherals

### `temperature_logger/` — Temperature Logger

Reads from a fake DHT22 sensor and logs to CSV. Colored output shows temperature ranges.

### `i2c_scanner/` — I2C Scanner

Scans the I2C bus and reports found mock devices. Demonstrates `Wire.h` usage.

### `wifi_scanner/` — WiFi Scanner

Scans for fake WiFi networks and displays them with signal strength bars.

## Visual Output

### `servo_sweep/` — Servo Sweep

Sweeps a servo from 0° to 180° with real-time ASCII visualization.

```
🔄 Servo:  45° [░░░░░░░░░░█░░░░░░░░░░░░░░░░░░░░░░░░░░░░░]
🔄 Servo:  90° [░░░░░░░░░░░░░░░░░░░░█░░░░░░░░░░░░░░░░░░░]
🔄 Servo: 135° [░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░█░░░░░░░░░]
```

### `lcd_display/` — LCD Display

Shows text on a simulated 16×2 LCD in the terminal.

```
┌──────────────────┐
│ Hello World!     │
│ Line 2 here      │
└──────────────────┘
```

### `neopixel_rainbow/` — NeoPixel Rainbow

Renders a color-cycling rainbow using ANSI 24-bit true-color escape codes.

## Running All Examples

```bash
# Run each one:
for dir in examples/*/; do
    echo "=== $dir ==="
    timeout 5 ./esp32emu run "$dir"/*.ino 2>/dev/null
done
```
