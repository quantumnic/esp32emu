#pragma once
#include <cstdint>
#include <cstdio>

class Adafruit_HTU31D {
public:
    Adafruit_HTU31D() {}

    bool begin(uint8_t addr = 0x40) { _addr = addr; _started = true; return true; }

    float readTemperature() const { return _temperature; }
    float readHumidity() const { return _humidity; }

    void enableHeater(bool en) { _heater = en; }
    bool isHeaterEnabled() const { return _heater; }

    void reset() { _temperature = 25.0f; _humidity = 50.0f; _heater = false; }

    uint32_t readSerial() const { return _serial; }

    // Diagnostic
    bool diagnosticStatus() const { return _diag_ok; }

    // Test helpers
    void _test_set_temperature(float t) { _temperature = t; }
    void _test_set_humidity(float h) { _humidity = h; }
    void _test_set_serial(uint32_t s) { _serial = s; }
    void _test_set_diagnostic(bool ok) { _diag_ok = ok; }

private:
    uint8_t _addr = 0x40;
    bool _started = false;
    bool _heater = false;
    bool _diag_ok = true;
    float _temperature = 25.0f;
    float _humidity = 50.0f;
    uint32_t _serial = 0x12345678;
};
