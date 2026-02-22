#pragma once
// esp32emu: TM1637 7-segment display mock
#include <cstdint>
#include <cstdio>
#include <cstring>

class TM1637Display {
public:
    TM1637Display(uint8_t clk, uint8_t dio) : _clk(clk), _dio(dio), _brightness(7), _colonOn(false) {
        memset(_segments, 0, sizeof(_segments));
    }

    void setBrightness(uint8_t brightness, bool on = true) {
        _brightness = brightness & 0x07;
        _on = on;
    }

    void setSegments(const uint8_t segments[], uint8_t length = 4, uint8_t pos = 0) {
        for (uint8_t i = 0; i < length && (pos + i) < 4; i++) {
            _segments[pos + i] = segments[i];
        }
    }

    void showNumberDec(int num, bool leading_zero = false, uint8_t length = 4, uint8_t pos = 0) {
        _lastNumber = num;
        _leadingZero = leading_zero;
        (void)length; (void)pos;
        printf("[esp32emu] TM1637: show %d\n", num);
    }

    void showNumberDecEx(int num, uint8_t dots = 0, bool leading_zero = false, uint8_t length = 4, uint8_t pos = 0) {
        _lastNumber = num;
        _colonOn = (dots & 0x40) != 0;
        _leadingZero = leading_zero;
        (void)length; (void)pos;
        printf("[esp32emu] TM1637: show %d (dots=0x%02X)\n", num, dots);
    }

    void clear() {
        memset(_segments, 0, sizeof(_segments));
        printf("[esp32emu] TM1637: cleared\n");
    }

    static uint8_t encodeDigit(uint8_t digit) {
        static const uint8_t table[] = {
            0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07,
            0x7F, 0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71
        };
        return (digit < 16) ? table[digit] : 0;
    }

    // Test helpers
    int getLastNumber() const { return _lastNumber; }
    bool isColonOn() const { return _colonOn; }
    uint8_t getBrightness() const { return _brightness; }
    bool isOn() const { return _on; }
    const uint8_t* getSegments() const { return _segments; }
    uint8_t getClkPin() const { return _clk; }
    uint8_t getDioPin() const { return _dio; }
    bool hasLeadingZero() const { return _leadingZero; }

private:
    uint8_t _clk, _dio;
    uint8_t _brightness;
    bool _on = true;
    bool _colonOn;
    bool _leadingZero = false;
    int _lastNumber = 0;
    uint8_t _segments[4];
};
