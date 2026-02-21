#pragma once
// esp32emu: Adafruit ILI9341 TFT display mock (240x320)
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

// Colors (RGB565)
#define ILI9341_BLACK       0x0000
#define ILI9341_NAVY        0x000F
#define ILI9341_DARKGREEN   0x03E0
#define ILI9341_DARKCYAN    0x03EF
#define ILI9341_MAROON      0x7800
#define ILI9341_PURPLE      0x780F
#define ILI9341_OLIVE       0x7BE0
#define ILI9341_LIGHTGREY   0xC618
#define ILI9341_DARKGREY    0x7BEF
#define ILI9341_BLUE        0x001F
#define ILI9341_GREEN       0x07E0
#define ILI9341_CYAN        0x07FF
#define ILI9341_RED         0xF800
#define ILI9341_MAGENTA     0xF81F
#define ILI9341_YELLOW      0xFFE0
#define ILI9341_WHITE       0xFFFF
#define ILI9341_ORANGE      0xFD20
#define ILI9341_GREENYELLOW 0xAFE5
#define ILI9341_PINK        0xF81F

class Adafruit_ILI9341 {
public:
    static constexpr int WIDTH = 240;
    static constexpr int HEIGHT = 320;

    Adafruit_ILI9341(int8_t cs, int8_t dc, int8_t rst = -1)
        : _cs(cs), _dc(dc), _rst(rst) {
        _framebuf.resize(WIDTH * HEIGHT, 0);
    }

    void begin(uint32_t freq = 0) {
        (void)freq;
        _initialized = true;
        printf("[esp32emu] ILI9341: initialized (CS=%d, DC=%d)\n", _cs, _dc);
    }

    void fillScreen(uint16_t color) {
        std::fill(_framebuf.begin(), _framebuf.end(), color);
        printf("[esp32emu] ILI9341: fillScreen(0x%04X)\n", color);
    }

    void drawPixel(int16_t x, int16_t y, uint16_t color) {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            _framebuf[y * WIDTH + x] = color;
        }
    }

    uint16_t readPixel(int16_t x, int16_t y) {
        if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
            return _framebuf[y * WIDTH + x];
        }
        return 0;
    }

    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
        // Bresenham
        int16_t dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int16_t dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int16_t err = dx + dy;
        while (true) {
            drawPixel(x0, y0, color);
            if (x0 == x1 && y0 == y1) break;
            int16_t e2 = 2 * err;
            if (e2 >= dy) { err += dy; x0 += sx; }
            if (e2 <= dx) { err += dx; y0 += sy; }
        }
    }

    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        drawLine(x, y, x + w - 1, y, color);
        drawLine(x, y + h - 1, x + w - 1, y + h - 1, color);
        drawLine(x, y, x, y + h - 1, color);
        drawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
    }

    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
        for (int16_t j = y; j < y + h; j++)
            for (int16_t i = x; i < x + w; i++)
                drawPixel(i, j, color);
    }

    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
        int16_t f = 1 - r, ddF_x = 1, ddF_y = -2 * r, x = 0, y = r;
        drawPixel(x0, y0 + r, color); drawPixel(x0, y0 - r, color);
        drawPixel(x0 + r, y0, color); drawPixel(x0 - r, y0, color);
        while (x < y) {
            if (f >= 0) { y--; ddF_y += 2; f += ddF_y; }
            x++; ddF_x += 2; f += ddF_x;
            drawPixel(x0+x, y0+y, color); drawPixel(x0-x, y0+y, color);
            drawPixel(x0+x, y0-y, color); drawPixel(x0-x, y0-y, color);
            drawPixel(x0+y, y0+x, color); drawPixel(x0-y, y0+x, color);
            drawPixel(x0+y, y0-x, color); drawPixel(x0-y, y0-x, color);
        }
    }

    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) {
        for (int16_t y = -r; y <= r; y++)
            for (int16_t x = -r; x <= r; x++)
                if (x*x + y*y <= r*r) drawPixel(x0+x, y0+y, color);
    }

    void setCursor(int16_t x, int16_t y) { _cursorX = x; _cursorY = y; }
    void setTextColor(uint16_t c) { _textColor = c; }
    void setTextColor(uint16_t c, uint16_t bg) { _textColor = c; _textBg = bg; }
    void setTextSize(uint8_t s) { _textSize = s; }

    void print(const char* text) {
        printf("[esp32emu] ILI9341: print(\"%s\") at (%d,%d)\n", text, _cursorX, _cursorY);
        _lastText = text;
        _cursorX += strlen(text) * 6 * _textSize;
    }

    void println(const char* text) {
        print(text);
        _cursorX = 0;
        _cursorY += 8 * _textSize;
    }

    void setRotation(uint8_t r) { _rotation = r & 3; }
    uint8_t getRotation() const { return _rotation; }
    int16_t width() const { return (_rotation & 1) ? HEIGHT : WIDTH; }
    int16_t height() const { return (_rotation & 1) ? WIDTH : HEIGHT; }

    void invertDisplay(bool i) { _inverted = i; }

    // esp32emu test helpers
    bool isInitialized() const { return _initialized; }
    const std::vector<uint16_t>& getFramebuffer() const { return _framebuf; }
    const char* getLastText() const { return _lastText.c_str(); }
    int16_t getCursorX() const { return _cursorX; }
    int16_t getCursorY() const { return _cursorY; }
    uint16_t getTextColor() const { return _textColor; }
    uint8_t getTextSize() const { return _textSize; }
    bool isInverted() const { return _inverted; }

private:
    int8_t _cs, _dc, _rst;
    bool _initialized = false;
    bool _inverted = false;
    uint8_t _rotation = 0;
    int16_t _cursorX = 0, _cursorY = 0;
    uint16_t _textColor = ILI9341_WHITE;
    uint16_t _textBg = ILI9341_BLACK;
    uint8_t _textSize = 1;
    std::vector<uint16_t> _framebuf;
    std::string _lastText;
};
