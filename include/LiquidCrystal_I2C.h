#pragma once
// esp32emu — LiquidCrystal_I2C mock
// Simulates an I2C character LCD (HD44780 compatible via PCF8574 I/O expander).

#include <cstdint>
#include <cstdio>
#include <cstring>

class LiquidCrystal_I2C {
public:
    LiquidCrystal_I2C(uint8_t addr, uint8_t cols, uint8_t rows)
        : addr_(addr), cols_(cols), rows_(rows) {
        clear();
    }

    void init() { begin(); }

    void begin() {
        initialized_ = true;
        clear();
        fprintf(stderr, "[esp32emu] LCD I2C: %dx%d at 0x%02X\n", cols_, rows_, addr_);
    }

    void clear() {
        memset(buffer_, ' ', sizeof(buffer_));
        cursor_col_ = 0;
        cursor_row_ = 0;
    }

    void home() { cursor_col_ = 0; cursor_row_ = 0; }

    void setCursor(uint8_t col, uint8_t row) {
        cursor_col_ = col < cols_ ? col : 0;
        cursor_row_ = row < rows_ ? row : 0;
    }

    size_t print(const char* s) {
        size_t n = 0;
        while (*s && cursor_col_ < cols_) {
            buffer_[cursor_row_][cursor_col_++] = *s++;
            n++;
        }
        return n;
    }

    size_t print(int val) {
        char buf[16];
        snprintf(buf, sizeof(buf), "%d", val);
        return print(buf);
    }

    size_t print(float val, int dec = 2) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.*f", dec, val);
        return print(buf);
    }

    size_t println(const char* s) {
        size_t n = print(s);
        cursor_col_ = 0;
        if (cursor_row_ < rows_ - 1) cursor_row_++;
        return n;
    }

    void backlight() { backlight_ = true; }
    void noBacklight() { backlight_ = false; }
    void display() { display_ = true; }
    void noDisplay() { display_ = false; }
    void blink() { blink_ = true; }
    void noBlink() { blink_ = false; }
    void cursor() { cursor_on_ = true; }
    void noCursor() { cursor_on_ = false; }
    void scrollDisplayLeft() { scroll_pos_--; }
    void scrollDisplayRight() { scroll_pos_++; }
    void autoscroll() { autoscroll_ = true; }
    void noAutoscroll() { autoscroll_ = false; }

    void createChar(uint8_t location, uint8_t charmap[]) {
        if (location < 8) {
            for (int i = 0; i < 8; i++) custom_chars_[location][i] = charmap[i];
        }
    }

    // ── Test helpers ──
    bool isInitialized() const { return initialized_; }
    bool isBacklight() const { return backlight_; }
    bool isDisplay() const { return display_; }
    uint8_t getCols() const { return cols_; }
    uint8_t getRows() const { return rows_; }
    uint8_t getCursorCol() const { return cursor_col_; }
    uint8_t getCursorRow() const { return cursor_row_; }

    // Get text on a specific row
    void getRow(uint8_t row, char* out, size_t len) const {
        if (row < rows_ && len > 0) {
            size_t n = cols_ < len - 1 ? cols_ : len - 1;
            memcpy(out, buffer_[row], n);
            out[n] = '\0';
        }
    }

    char getChar(uint8_t col, uint8_t row) const {
        if (col < cols_ && row < rows_) return buffer_[row][col];
        return ' ';
    }

private:
    uint8_t addr_, cols_, rows_;
    bool initialized_ = false;
    bool backlight_ = true;
    bool display_ = true;
    bool blink_ = false;
    bool cursor_on_ = false;
    bool autoscroll_ = false;
    int scroll_pos_ = 0;
    uint8_t cursor_col_ = 0;
    uint8_t cursor_row_ = 0;
    char buffer_[4][40] = {};   // max 4 rows x 40 cols
    uint8_t custom_chars_[8][8] = {};
};
