// Adafruit_HTU21DF.h — HTU21D-F humidity/temperature sensor mock for esp32emu
#pragma once
#include <cstdint>
#include <cstdio>

namespace htu21df_test {
    inline float& temperature() { static float v = 22.5f; return v; }
    inline float& humidity()    { static float v = 45.0f; return v; }
    inline bool&  available()   { static bool  v = true;  return v; }
    inline void reset() { temperature() = 22.5f; humidity() = 45.0f; available() = true; }
    inline void setTemperature(float t) { temperature() = t; }
    inline void setHumidity(float h) { humidity() = h; }
    inline void setAvailable(bool a) { available() = a; }
}

class Adafruit_HTU21DF {
public:
    Adafruit_HTU21DF() : _begun(false) {}

    bool begin() {
        if (!htu21df_test::available()) return false;
        _begun = true;
        printf("[esp32emu] HTU21DF: initialized\n");
        return true;
    }

    float readTemperature() const {
        if (!_begun) return 0.0f;
        return htu21df_test::temperature();
    }

    float readHumidity() const {
        if (!_begun) return 0.0f;
        return htu21df_test::humidity();
    }

    void reset() {
        printf("[esp32emu] HTU21DF: reset\n");
    }

    bool isConnected() const { return _begun && htu21df_test::available(); }

private:
    bool _begun;
};
