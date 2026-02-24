# Changelog

All notable changes to esp32emu are documented here.

## [v1.18.0] — 2026-02-24

### Added
- **PCF8575** 16-bit I2C I/O Expander mock (write16/read16, pin toggle, polarity)
- **Adafruit_CAP1188** 8-channel capacitive touch sensor mock (touch bitmask, delta, sensitivity, multi-touch)
- **Adafruit_ENS160** digital multi-gas air quality sensor mock (AQI, TVOC, eCO2, temperature/humidity compensation)
- **Adafruit_VEML6070** UV light sensor mock (raw UV, integration time, sleep/wake)
- **SparkFun Thing Plus C** board support (ESP32-WROOM-32E, Qwiic, USB-C, µSD, LiPo)
- 4 new tests: `test_pcf8575`, `test_adafruit_cap1188`, `test_adafruit_ens160`, `test_adafruit_veml6070`
- 4 new examples: `pcf8575_io_expander`, `cap1188_touch_pads`, `ens160_air_quality`, `veml6070_uv_sensor`
- **287 tests, 294 examples** total

## [v1.17.0] — 2026-02-24

### Added
- **Adafruit MPL3115A2 mock** — Altimeter/barometer/temperature sensor, oversample control, mode switching, sea-level pressure config, test helpers
- **Si5351 clock generator mock** — Triple-output programmable clock (8kHz–160MHz), PLL config, drive strength (2–8mA), frequency correction, output enable
- **ADS1220 24-bit ADC mock** — 4-channel MUX, gain 1–128x, data rate config, continuous/single-shot mode, internal temperature sensor, power management
- **UM TinyS3 board** — Unexpected Maker TinyS3 with ESP32-S3, 8MB Flash, 8MB PSRAM, USB-C, ASCII art
- **mpl3115a2_weather example** — Weather station with pressure, altitude, temperature display
- **si5351_vfo example** — Variable Frequency Oscillator for ham radio bands
- **ads1220_loadcell example** — Precision load cell / scale reading with 128x gain

### Stats
- **283 tests**, **290 examples**, **37 boards**

---

## [v1.16.0] — 2026-02-24

### Added
- **AsyncMqttClient test** — Full test coverage for async MQTT client mock (connect, disconnect, subscribe, unsubscribe, publish, message injection, reset, chaining)
- **BluetoothSerial test** — Comprehensive test for BT Serial SPP mock (begin/end, connect/disconnect, read/write, readString, readStringUntil, readBytes, inject, TX data)
- **ESPmDNS enhanced mock** — Added service query/discovery with `queryService()`, `hostname()`, `IP()`, `port()`, `txt()`, `txtCount()`, test helpers (`_addQueryResult`, `_clearQueryResults`, `_reset`)
- **ESPmDNS test** — Full test for mDNS hostname, service registration, TXT records, service discovery with injected results
- **QT Py ESP32-C3 board** — Adafruit QT Py ESP32-C3 with STEMMA QT, 11 GPIO, NeoPixel, ASCII art
- **TinyPICO board** — Unexpected Maker TinyPICO with ESP32-PICO-D4, APA102 LED, LiPo, ASCII art
- **ESP32 DevKit V1 board** — Classic ESP32-WROOM-32 development board with 38 GPIO, ASCII art
- **mqtt_async_sensor example** — Async MQTT temperature/humidity publisher
- **bt_serial_bridge example** — Bidirectional Bluetooth Serial ↔ USB Serial bridge
- **mdns_discovery example** — mDNS hostname registration, service advertisement, and network discovery

### Stats
- **280 tests**, **287 examples**, **36 boards**

## [v1.15.0] — 2026-02-24

### Added
- **Adafruit_ST7735** — ST7735 TFT display mock with initR/initB, tab types (green/red/black/mini), rotation, sleep, invert, pixel/rect drawing, color565, framebuffer access
- **Adafruit_BME68x** — BME68x (BSEC2-compatible) sensor mock with temperature/pressure/humidity/gas resistance, oversampling config, IIR filter, gas heater, altitude calculation, async reading flow
- **M5StickC Plus** board — ESP32-PICO-D4 with WiFi + BT, LED pin 10
- **TTGO T-Display** board — ESP32-D0WDQ6 with WiFi + BT, 240MHz
- **XIAO ESP32-C6** board (Seeed Studio) — ESP32-C6, WiFi + BT, 160MHz, LED pin 15
- 3 new tests (`test_st7735`, `test_bme68x`, `test_boards_new`) — **277 tests total**
- 3 new examples (`st7735_graphics`, `bme68x_air_quality`, `m5stickc_plus_demo`) — **284 examples total**

## [v1.14.0] — 2026-02-24

### Added
- **INA226** power monitor mock — bus voltage, shunt voltage, current, power calculation, configurable averaging (1–1024), conversion times, operating modes, calibration with custom shunt resistance, over-current and over-voltage alert thresholds, reset
- **MAX44009** ambient light sensor mock — lux reading (0.045–188,000 lux), manual/continuous mode, integration time (6ms–800ms), high/low thresholds, interrupt enable, threshold timer, error injection
- **Adafruit BusIO** — `Adafruit_I2CDevice` and `Adafruit_SPIDevice` transport abstraction mocks with read/write/write_then_read, prefix support, test data injection for both I2C and SPI
- **LOLIN S3** board — Wemos LOLIN S3 (ESP32-S3, 46 GPIO, 16MB Flash, WiFi/BT5, USB-C) with ASCII art
- New tests: `test_ina226`, `test_max44009`, `test_adafruit_busio` (274 total)
- New examples: `ina226_power_monitor`, `max44009_light_meter` (281 total)

## [v1.13.0] — 2026-02-24

### Added
- **ACS712** Hall-effect current sensor mock — 5A/20A/30A variants, DC/AC current reading, calibration, midpoint, voltage, sensitivity config
- **HDC1080** temperature & humidity sensor mock — 14/11/8-bit resolution, heater control, manufacturer/device IDs, battery status
- **INA3221** triple-channel power monitor mock — per-channel bus voltage/current/power, shunt resistance, enable/disable channels, averaging mode, critical/warning alerts
- **M5Atom Lite** board support — ESP32-PICO-D4 with WiFi, BT, SK6812 LED, 26 GPIO, ASCII diagram
- 4 new examples: `acs712_current_meter`, `hdc1080_climate`, `ina3221_power_monitor`, `m5atom_lite_blink`
- 268 tests, 275 examples, 28 boards

## [v1.12.0] — 2026-02-24

### Added
- **Adafruit DRV2605** haptic motor driver mock — waveform slots, ERM/LRA modes, internal trigger, realtime playback, library selection, play/stop
- **Adafruit LPS22** barometric pressure sensor mock — I2C/SPI init, pressure/temperature reading, data rate config, data-ready flags, reset
- **Adafruit MPRLS** ported pressure sensor mock — pressure reading (hPa), status byte, configurable reset/EOC pins, PSI range
- **Adafruit HTU31D** temperature & humidity sensor mock — temp/humidity reading, heater control, reset, serial number, diagnostic status
- **Adafruit Feather ESP32-C6** board support — ESP32-C6 RISC-V with WiFi 6, BT5, Zigbee/Thread, LiPo, STEMMA QT, ASCII diagram
- 4 new examples: `drv2605_haptic`, `lps22_barometer`, `mprls_pressure`, `htu31d_climate`
- 265 tests, 271 examples, 27 boards

## [v1.11.0] — 2026-02-24

### Added
- **Sensirion SCD4x** (SCD40/SCD41) CO2 sensor mock — periodic/low-power/single-shot measurement, temperature offset, altitude/pressure compensation, forced recalibration, ASC, power management, serial number
- **Adafruit LIS3MDL** 3-axis magnetometer mock — configurable range (4-16 Gauss), data rate, operation/performance modes, interrupt config, compass heading calculation
- **Adafruit ADXL345** 3-axis accelerometer mock — range (2-16G), data rate, tap/double-tap detection, free-fall, activity/inactivity, pitch/roll calculation
- **DFRobotDFPlayerMini** MP3 player mock — play/pause/stop, next/previous, volume (0-30), EQ, folder/track playback, loop, DAC, sleep/wake, message events
- 4 new examples: `scd4x_co2_monitor`, `lis3mdl_compass`, `adxl345_motion_detect`, `dfplayer_jukebox`
- 261 tests, 267 examples

## [v1.10.0] — 2026-02-23

### Added
- **Adafruit AS7341** 11-channel spectral color sensor mock — F1-F8 (415-680nm), Clear, NIR channels, gain/ATIME/ASTEP config, flicker detection, LED control, bulk read
- **Adafruit PMSA003I** (PM25AQI) particulate matter sensor mock — PM1.0/PM2.5/PM10.0 standard & environmental, 6 particle size bins, I2C & UART init
- **MCP9600** thermocouple EMF converter mock — hot/cold junction temps, delta, K/J/T/N/S/E/B/R types, 4 alert thresholds, filter coefficient, ADC resolution, device ID
- **Olimex ESP32-PoE** board support — ESP32-D0WDQ6 with Ethernet (PoE), 40 GPIO, WiFi+BT, ASCII diagram
- 3 new examples: `as7341_spectral`, `pmsa003i_airquality`, `mcp9600_thermocouple`
- 253 tests, 258 examples, 25 boards

## [v1.9.0] — 2026-02-22

### Added
- **Adafruit ST7789** TFT display mock — 240×320 SPI TFT with framebuffer, pixel read-back, rotation, invert, sleep, color565, HLine/VLine
- **MAX17048** LiPo fuel gauge mock — voltage, SOC, charge rate, alert threshold (1-32%), hibernate/sleep modes, quick start, reset
- **Adafruit LC709203F** battery fuel gauge mock — voltage, SOC, pack size/APA, power modes, temperature modes, RSOC alarm, cell profile, thermistor B
- **Adafruit LSM9DS1** 9-DOF IMU mock — accelerometer, gyroscope, magnetometer with configurable ranges/gains/scales, event API, temperature
- **Adafruit TCS34725** color sensor mock — RGB+clear raw data, lux calculation, color temperature (McCamy's), gain/integration time, interrupt/limits
- 5 new examples: st7789_demo, max17048_demo, lc709203f_demo, lsm9ds1_demo, tcs34725_demo
- Total: 207 tests, 205 examples

## [v1.8.0] — 2026-02-22

### Added
- **TMC2209** stepper driver mock — UART config, StealthChop/CoolStep, StallGuard, velocity control, current settings
- **Adafruit AMG88xx** thermal camera mock — 8x8 IR pixel array, thermistor, frame rate, power modes, interrupts
- **PID_v1** controller mock — PID auto/manual mode, output limits, sample time, P_ON_M/P_ON_E, DIRECT/REVERSE
- **MQ135** gas sensor mock — air quality PPM, temperature/humidity correction, RZero calibration
- **ESP Console** test — command registration, dispatch, help, argument parsing
- **Temperature Sensor** test — ESP-IDF internal temp sensor install/enable/read/inject
- 6 new examples: esp_console_cli, tmc2209_stepper, amg88xx_thermal, pid_temperature, mq135_air_quality, temp_sensor_internal
- Total: **189 tests**, **183 examples**

## [v1.7.0] — 2026-02-21

### Added
- **MFRC522 RFID Reader** mock — card detection, UID read, MIFARE authentication, block read/write, PICC type identification
- **Adafruit MLX90614** IR thermometer mock — contactless object/ambient temperature in °C/°F, emissivity control
- **CCS811 Air Quality Sensor** mock — eCO2/TVOC readings, drive modes, baseline calibration, environmental compensation
- **RCSwitch 433MHz RF** mock — transmit/receive RF codes, protocol/timing config, Type A/B switch helpers
- **AS5600 Magnetic Rotary Encoder** mock — 12-bit angle, degrees, magnet status, AGC, cumulative position, direction
- 5 new examples: `rfid_access`, `mlx90614_ir_temp`, `ccs811_air_quality`, `rcswitch_rf433`, `as5600_encoder`
- 5 new tests: `test_mfrc522`, `test_mlx90614`, `test_ccs811`, `test_rcswitch`, `test_as5600`
- **145 tests, 140 examples** total

## [v1.6.0] — 2026-02-19

### Added
- **FreeRTOS Software Timers** (`freertos/timers.h`) — `xTimerCreate`, `xTimerStart`, `xTimerStop`, `xTimerReset`, `xTimerChangePeriod`, `xTimerDelete`, `pvTimerGetTimerID`, `pcTimerGetName`, `xTimerIsTimerActive`, ISR variants, and `timer_test::` helpers for fire count tracking
- **FreeRTOS Task Notifications** — `xTaskNotify`, `xTaskNotifyGive`, `ulTaskNotifyTake`, `xTaskNotifyWait`, `xTaskGetCurrentTaskHandle`, with `eSetBits`, `eIncrement`, `eSetValueWithOverwrite`, `eSetValueWithoutOverwrite` actions
- 2 new examples: `freertos_timer_demo` (periodic + one-shot timers), `task_notify_demo` (producer/consumer with notifications)

### Stats
- **13 boards**, **52 library mocks** (was 50): +FreeRTOS software timers, task notifications
- **69 tests** (was 67): +test_freertos_timers, test_task_notify
- **66 examples** (was 64): +freertos_timer_demo, task_notify_demo

## [v1.5.0] — 2026-02-19

### Added
- **ESP-IDF logging macros** (`esp_log.h`) — `ESP_LOGE`/`W`/`I`/`D`/`V` with colored terminal output, log level filtering via `esp_log_level_set()`, message counting, and callback hook for test interception
- **ESP-IDF high-resolution timer** (`esp_timer.h`) — `esp_timer_create`, `start_periodic`, `start_once`, `stop`, `delete`, `esp_timer_get_time()` (microsecond precision), `esp_timer_is_active()`, and `esp_timer_test::getFireCount()`/`resetFireCount()` test helpers
- **LEDC PWM controller mock** (`esp32emu_ledc.h`) — Full channel state tracking (16 channels), frequency/duty/pin readback, `ledcWriteTone`/`ledcWriteNote` with frequency calculation, and `ledc_test::` helpers (replaces previous inline stubs)
- 3 new examples: `pwm_fade` (LED fade with LEDC), `esp_log_demo` (logging levels), `esp_timer_blink` (timer-driven blink)

### Improved
- **LEDC test** — Enhanced from basic smoke test to comprehensive channel state, multi-channel, and writeNote frequency verification

### Stats
- **12 boards**, **48 library mocks** (was 45): +esp_log, esp_timer, LEDC (full)
- **47 tests** (was 45): +test_esp_log, test_esp_timer (test_ledc enhanced)
- **45 examples** (was 42): +pwm_fade, esp_log_demo, esp_timer_blink

## [v1.4.0] — 2026-02-19

### Added
- **FastLED library mock** (`FastLED.h`) — WS2812B/APA102 LED strip emulation with `CRGB`, `CHSV`, `hsv2rgb_rainbow`, `fill_rainbow`, `fill_solid`, `fadeToBlackBy`, `nscale8`, brightness control, show counting, and named colors
- **ArduinoJson v7 mock** (`ArduinoJson.h`) — Full JSON serialize/deserialize with `JsonDocument`, `JsonObject`, `JsonArray`, nested objects/arrays, `serializeJson`/`deserializeJson`, error handling, and `containsKey`
- **ESP32 TWAI/CAN bus mock** (`driver/twai.h`) — CAN controller emulation with `twai_driver_install/uninstall`, `twai_start/stop`, `twai_transmit/receive`, standard/extended frames, timing configs (25K–1Mbps), filter config, status info, and `twai_test::injectRxMessage()`/`getTxLog()` test helpers
- 3 new examples: `fastled_rainbow` (rainbow LED animation), `json_api` (REST API with JSON), `can_bus` (CAN frame send/receive)

### Fixed
- **Ticker test timing flake** — Increased sleep margins in test_ticker to prevent CI failures under load

### Stats
- **12 boards**, **45 library mocks** (was 42): +FastLED, ArduinoJson, TWAI/CAN
- **45 tests** (was 42): +test_fastled, test_arduinojson, test_twai
- **42 examples** (was 39): +fastled_rainbow, json_api, can_bus

## [v1.3.0] — 2026-02-18

### Added
- **DHT sensor library mock** (`DHT.h`) — DHT11/DHT22/AM2301 temperature & humidity sensor with `readTemperature()` (°C/°F), `readHumidity()`, `computeHeatIndex()` (Rothfusz regression), and `setTemperature()`/`setHumidity()` test helpers
- **Adafruit SSD1306 OLED display mock** (`Adafruit_SSD1306.h`) — 128x64/128x32 OLED emulation with `drawPixel`, `drawRect`, `fillCircle`, `drawLine`, `drawBitmap`, text rendering (`print`/`println`), pixel buffer, and test helpers (`getPrintedLines()`, `getPixel()`, `getDisplayCount()`, `getDrawCalls()`)
- **MPU6050 IMU mock** (`MPU6050.h`) — 6-axis accelerometer/gyroscope with `getMotion6()`, individual axis accessors, G-force conversion (±2g), degrees/second conversion (±250°/s), pitch/roll calculation, temperature, and test helpers
- 3 new examples: `dht_monitor` (DHT22 readings), `oled_dashboard` (SSD1306 system dashboard), `mpu6050_tilt` (tilt angle display)

### Stats
- **12 boards**, **42 library mocks** (was 39): +DHT, Adafruit_SSD1306, MPU6050
- **42 tests** (was 39): +test_dht, test_ssd1306, test_mpu6050
- **39 examples** (was 36): +dht_monitor, oled_dashboard, mpu6050_tilt

## [v1.2.0] — 2026-02-18

### Added
- **Stepper motor library mock** (`Stepper.h`) — Full Arduino Stepper library emulation with 2-wire and 4-wire constructors, position tracking, angle calculation, speed setting, and test helpers (`getPosition()`, `getTotalSteps()`, `getAngle()`, `resetPosition()`)
- **IRremote library mock** (`IRremote.h`) — Infrared send/receive emulation with `IRrecv` (decode queue, `injectCode()` test helper) and `IRsend` (NEC, Sony, Samsung, RC5, raw protocols, sent log tracking)
- **Adafruit BMP280 sensor mock** (`Adafruit_BMP280.h`) — BMP280 pressure/temperature sensor with `readTemperature()`, `readPressure()`, `readAltitude()`, barometric formula, and `setTemperature()`/`setPressure()` test helpers
- 3 new examples: `stepper_sweep` (motor back-and-forth), `ir_remote_control` (IR send/receive echo), `bmp280_weather` (weather station readings)

### Fixed
- **SPI.h unused field warnings** — Added getter methods for `SPISettings` private fields, eliminating `-Wunused-private-field` warnings

### Stats
- **12 boards**, **39 library mocks** (was 36): +Stepper, IRremote, Adafruit_BMP280
- **39 tests** (was 36): +test_stepper, test_irremote, test_bmp280
- **36 examples** (was 33): +stepper_sweep, ir_remote_control, bmp280_weather

## [v1.1.0] — 2026-02-18

### Added
- **Touch pin mock** (`esp32emu_touch.h`) — ESP32 capacitive touch support with `touchRead()`, `touchAttachInterrupt()`, `touchDetachInterrupt()`, threshold-based interrupt firing, and test helpers (`setTouchValue()`, `simulateTouch()`, `releaseTouch()`)
- **I2S audio mock** (`I2S.h`) — Full ESP-IDF I2S driver emulation with `i2s_driver_install/uninstall`, `i2s_start/stop`, `i2s_write/read`, `i2s_set_pin`, configurable sample rate/bit depth, and test helpers (`injectRxData()`, `getTxData()`, `getTxSize()`)
- **GPIO interrupt test** (`test_interrupt`) — Comprehensive test for `attachInterrupt`/`detachInterrupt` covering RISING, FALLING, CHANGE modes and edge cases
- **CONTRIBUTING.md** — Contributing guide with mock design guidelines, code style, testing and example conventions
- 2 new examples: `touch_piano` (capacitive touch piano with 7 keys), `i2s_tone` (440Hz sine wave I2S output)

### Stats
- **12 boards**, **36 library mocks** (was 33): +Touch pins, I2S, GPIO interrupts (test)
- **36 tests** (was 33): +test_touch, test_i2s, test_interrupt
- **33 examples** (was 31): +touch_piano, i2s_tone

## [v1.0.0] — 2026-02-18

### 🎉 v1.0 Release!

### Added
- **Ticker library mock** — Non-blocking timer callbacks with `attach()`, `attach_ms()`, `once()`, `once_ms()`, `detach()`, `active()` and real threaded execution
- **OneWire library mock** — Full bus emulation with device search, read/write, CRC8, and `addDevice()`/`injectReadData()` test helpers
- **DallasTemperature library mock** — DS18B20 temperature sensor support with multi-sensor discovery, °C/°F conversion, resolution config, and `setTemperature()`/`setTemperatureByIndex()` test helpers
- **ESP32 Task Watchdog Timer mock** (`esp_task_wdt.h`) — `esp_task_wdt_init/deinit/add/delete/reset` with timeout tracking, feed counting, and `simulateTimeout()` test helper
- 3 new tests: `test_ticker`, `test_onewire`, `test_wdt`
- 3 new examples: `ticker_blink` (non-blocking LED blink), `ds18b20_temp` (temperature sensor reading), `watchdog_demo` (WDT lifecycle)

### Stats
- **12 boards**, **33 library mocks** (was 29): +Ticker, OneWire, DallasTemperature, esp_task_wdt
- **33 tests** (was 30): +test_ticker, test_onewire, test_wdt
- **31 examples** (was 28): +ticker_blink, ds18b20_temp, watchdog_demo

## [v0.9.0] — 2026-02-18

### Added
- **ESP32-H2 board** — RISC-V core at 96MHz, BLE 5.0 + IEEE 802.15.4 (Thread/Zigbee), no WiFi, 22 GPIOs
- **Teensy 4.0 board** — NXP IMXRT1062 ARM Cortex-M7 at 600MHz, 1MB RAM, 40 digital + 14 analog pins
- **STM32 Blue Pill board** — STM32F103C8 ARM Cortex-M3 at 72MHz, 20KB RAM, 37 digital + 10 analog pins
- **ESPAsyncWebServer mock** — Full async web server emulation with route registration, request/response objects, query params, headers, redirects, not-found handler, wildcard URI matching, and `simulateRequest()` test helper
- **mDNS test** — Test coverage for ESPmDNS mock
- **SoftwareSerial test** — Test coverage for SoftwareSerial mock with inject/read verification
- 3 new examples: `async_rest_api` (async JSON API), `teensy_blink` (Teensy 4.0 LED chase), `bluepill_serial` (STM32 serial bridge)
- ASCII art for all 3 new boards
- Updated `--list-boards` to include all 12 boards

### Stats
- **12 boards** (was 9): +ESP32-H2, Teensy 4.0, STM32 Blue Pill
- **29 library mocks** (was 28): +ESPAsyncWebServer
- **30 tests** (was 27): +test_asyncwebserver, test_mdns, test_softserial
- **28 examples** (was 25): +async_rest_api, teensy_blink, bluepill_serial

## [v0.8.0] — 2026-02-18

### Added
- **ESP32-C6 board** — WiFi 6, BLE 5.0, RISC-V core at 160MHz with 31 GPIOs and ASCII art
- **RP2040 Pico board** — Raspberry Pi Pico support, dual-core Cortex-M0+ at 133MHz, GP0-GP29, LED on GP25
- **WiFiServer mock** — Real TCP server on loopback with `begin()`, `available()`, `stop()`. Accepts incoming WiFiClient connections
- **DNSServer mock** — Captive portal DNS redirection with wildcard/specific domain resolution, test helpers (`resolve`, `getProcessCount`)
- **WiFiClient move semantics** — Proper move constructor/assignment, preventing double-close bugs
- **`--list-boards` CLI flag** — Lists all supported boards with specs
- **`--help` CLI flag** — Usage information
- 3 new tests: `test_boards`, `test_dnsserver`, `test_wifiserver`
- 3 new examples: `captive_portal_dns` (DNS captive portal), `tcp_echo_server` (WiFiServer echo), `pico_blink` (RP2040 blink)

### Stats
- **9 boards** (was 7): +ESP32-C6, RP2040 Pico
- **28 library mocks** (was 26)
- **27 tests** (was 24)
- **25 examples** (was 22)

## [v0.7.0] — 2026-02-18

### Added
- **WiFiUDP mock** — Real UDP sockets on loopback for NTP, DNS, and general UDP communication. Full send/receive API with `beginPacket`/`endPacket`/`parsePacket`
- **ESP-NOW mock** — Peer-to-peer communication emulation with peer management, send/receive callbacks, broadcast support, and test helpers (`injectRecv`, `getSentLog`)
- **esp_sleep mock** — Deep sleep and light sleep emulation that records state instead of exiting. Timer wakeup, ext0/ext1 wakeup config, sleep callbacks for testing
- 3 new tests: `test_udp`, `test_espnow`, `test_sleep`
- 3 new examples: `udp_ntp` (NTP client), `espnow_sender` (sensor data P2P), `deep_sleep_timer` (timer wakeup)

### Stats
- **26 library mocks** (was 23)
- **24 tests** (was 21)
- **22 examples** (was 19)

## [v0.6.0] — 2026-02-18

### Added
- **PubSubClient (MQTT) mock** — Full publish/subscribe emulation with local message delivery, wildcard topic matching, and test helpers (`injectMessage`, `getPublished`)
- **BluetoothSerial mock** — ESP32 Bluetooth SPP emulation with ring buffer, `readString`/`readStringUntil`, TX inspection, and connection simulation
- **ArduinoOTA mock** — OTA update lifecycle simulation with callbacks (`onStart`, `onEnd`, `onProgress`, `onError`) and `simulateOTA()` test helper
- 3 new tests: `test_mqtt`, `test_bluetooth`, `test_ota`
- 2 new examples: `mqtt_sensor` (publish temps + subscribe to LED commands), `bluetooth_echo` (BT Serial echo server)
- This CHANGELOG

### Stats
- **23 library mocks** (was 20)
- **21 tests** (was 18)
- **19 examples** (was 17)

## [v0.5.0] — 2026-02-18

### Added
- SD card library mock (host filesystem backed)
- SPIFFS & LittleFS filesystem mocks
- Adafruit NeoPixel mock with terminal color visualization
- `tone()` / `noTone()` / `pulseIn()` functions
- 4 new tests, 2 new examples

## [v0.4.0] — 2026-02-18

### Added
- ESP32-S2 and Arduino Nano board support
- FreeRTOS task API (`xTaskCreate`, mutexes, semaphores) backed by `std::thread`
- 14 new tests

## [v0.3.0] — 2026-02-18

### Added
- ESP32-C3 board support (RISC-V)
- `Preferences.h` library mock
- LEDC PWM API
- CI with GitHub Actions
- 5 new tests

## [v0.2.0] — 2026-02-17

### Added
- Arduino Uno and Mega board support with terminal visualization
- `--board` CLI flag
- WiFi capability checks for non-WiFi boards
- 12 new examples

## [v0.1.0] — 2026-02-17

### Added
- Initial release
- ESP32 and ESP32-S3 emulation
- Real TCP/HTTP sockets (WebServer, WiFiClient, HTTPClient)
- GPIO, I2C, SPI, EEPROM, Servo, LCD mocks
- Rich terminal UI with pin visualization
