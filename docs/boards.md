# Board Support

esp32emu supports 4 boards. Select one with the `--board` flag.

## Board Comparison

| Feature | ESP32 | ESP32-S3 | Arduino Uno | Arduino Mega |
|---------|:-----:|:--------:|:-----------:|:------------:|
| **Chip** | ESP32 | ESP32-S3 | ATmega328P | ATmega2560 |
| **Digital Pins** | 40 | 48 | 14 | 54 |
| **Analog Pins** | 8 | 10 | 6 | 16 |
| **RAM** | 320 KB | 512 KB | 2 KB | 8 KB |
| **Flash** | 4 MB | 8 MB | 32 KB | 256 KB |
| **WiFi** | ✅ | ✅ | ❌ | ❌ |
| **Bluetooth** | ✅ | ✅ | ❌ | ❌ |
| **LED_BUILTIN** | 2 | 2 | 13 | 13 |

## Usage

```bash
# Default: ESP32
./esp32emu run sketch.ino

# Explicit board selection
./esp32emu run --board esp32 sketch.ino
./esp32emu run --board esp32s3 sketch.ino
./esp32emu run --board uno sketch.ino
./esp32emu run --board mega sketch.ino
```

## ESP32 (Default)

The standard ESP32 DevKit configuration. Full WiFi and Bluetooth support. 40 GPIO pins with 8 ADC channels.

**Best for:** WiFi projects, WebServer demos, REST APIs, MQTT, OTA updates.

## ESP32-S3

Enhanced ESP32 variant with more pins and memory. Suitable for projects needing extra I/O.

**Best for:** Same as ESP32, plus projects needing more GPIO or memory.

## Arduino Uno

The classic Arduino board. Limited pins and memory, no networking.

**Best for:** Basic GPIO, servo, LCD, I2C projects. Learning fundamentals.

**Limitations:**
- WiFi/Bluetooth calls log a warning and are no-ops
- Only 14 digital pins (D0–D13) and 6 analog (A0–A5)
- 2 KB RAM — the emulator doesn't enforce memory limits, but pin limits are real

## Arduino Mega

More pins and memory than the Uno, but still no networking.

**Best for:** Projects needing many I/O pins, multiple servos, complex wiring.

**Limitations:**
- Same WiFi/Bluetooth restrictions as Uno
- 54 digital pins, 16 analog pins

## Pin Validation

The emulator validates pin numbers against the selected board:

```cpp
// On Arduino Uno, this triggers a warning:
pinMode(20, OUTPUT);  // ⚠️ Pin 20 exceeds Uno's 14 digital pins

// On ESP32, this works fine:
pinMode(20, OUTPUT);  // ✅ ESP32 has 40 digital pins
```

Warnings are printed to stderr but execution continues (matching real Arduino behavior where out-of-range pins are silently ignored).
