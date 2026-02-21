#pragma once
// esp32emu: NeoPixelBus mock — alternative addressable LED library
#include <cstdint>
#include <cstdio>
#include <vector>
#include <algorithm>

struct RgbColor {
    uint8_t R = 0, G = 0, B = 0;
    RgbColor() = default;
    RgbColor(uint8_t r, uint8_t g, uint8_t b) : R(r), G(g), B(b) {}
    bool operator==(const RgbColor& o) const { return R == o.R && G == o.G && B == o.B; }
    bool operator!=(const RgbColor& o) const { return !(*this == o); }
    static RgbColor LinearBlend(const RgbColor& a, const RgbColor& b, float progress) {
        float p = std::max(0.0f, std::min(1.0f, progress));
        return RgbColor(
            (uint8_t)(a.R + (b.R - a.R) * p),
            (uint8_t)(a.G + (b.G - a.G) * p),
            (uint8_t)(a.B + (b.B - a.B) * p)
        );
    }
};

struct RgbwColor {
    uint8_t R = 0, G = 0, B = 0, W = 0;
    RgbwColor() = default;
    RgbwColor(uint8_t r, uint8_t g, uint8_t b, uint8_t w = 0) : R(r), G(g), B(b), W(w) {}
    bool operator==(const RgbwColor& o) const { return R == o.R && G == o.G && B == o.B && W == o.W; }
};

struct HslColor {
    float H = 0, S = 0, L = 0;
    HslColor() = default;
    HslColor(float h, float s, float l) : H(h), S(s), L(l) {}
    HslColor(const RgbColor& c) {
        float r = c.R / 255.0f, g = c.G / 255.0f, b = c.B / 255.0f;
        float mx = std::max({r, g, b}), mn = std::min({r, g, b});
        L = (mx + mn) / 2.0f;
        if (mx == mn) { H = S = 0; return; }
        float d = mx - mn;
        S = L > 0.5f ? d / (2.0f - mx - mn) : d / (mx + mn);
        if (mx == r) H = (g - b) / d + (g < b ? 6.0f : 0.0f);
        else if (mx == g) H = (b - r) / d + 2.0f;
        else H = (r - g) / d + 4.0f;
        H /= 6.0f;
    }
};

// NeoPixelBus template — simplified for esp32emu
// T_COLOR_FEATURE and T_METHOD are ignored; we just store RGB
template<typename T_COLOR_FEATURE = void, typename T_METHOD = void>
class NeoPixelBus {
public:
    NeoPixelBus(uint16_t count, uint8_t pin = 2) : pin_(pin) {
        pixels_.resize(count, RgbColor(0, 0, 0));
    }

    void Begin() {
        initialized_ = true;
        printf("[esp32emu] NeoPixelBus: %zu LEDs on pin %d\n", pixels_.size(), pin_);
    }

    void Show() {
        dirty_ = false;
        showCount_++;
    }

    bool CanShow() const { return true; }
    bool IsDirty() const { return dirty_; }

    void SetPixelColor(uint16_t index, const RgbColor& color) {
        if (index < pixels_.size()) { pixels_[index] = color; dirty_ = true; }
    }

    RgbColor GetPixelColor(uint16_t index) const {
        if (index < pixels_.size()) return pixels_[index];
        return RgbColor(0, 0, 0);
    }

    uint16_t PixelCount() const { return (uint16_t)pixels_.size(); }

    void ClearTo(const RgbColor& color) {
        for (auto& p : pixels_) p = color;
        dirty_ = true;
    }

    void SetBrightness(uint8_t b) { brightness_ = b; }
    uint8_t GetBrightness() const { return brightness_; }

    void RotateLeft(uint16_t count = 1) {
        if (pixels_.empty()) return;
        count %= pixels_.size();
        std::rotate(pixels_.begin(), pixels_.begin() + count, pixels_.end());
        dirty_ = true;
    }

    void RotateRight(uint16_t count = 1) {
        if (pixels_.empty()) return;
        count %= pixels_.size();
        std::rotate(pixels_.rbegin(), pixels_.rbegin() + count, pixels_.rend());
        dirty_ = true;
    }

    void ShiftLeft(uint16_t count = 1) {
        RotateLeft(count);
        for (uint16_t i = (uint16_t)(pixels_.size() - count); i < pixels_.size(); i++)
            pixels_[i] = RgbColor(0, 0, 0);
    }

    void ShiftRight(uint16_t count = 1) {
        RotateRight(count);
        for (uint16_t i = 0; i < count && i < pixels_.size(); i++)
            pixels_[i] = RgbColor(0, 0, 0);
    }

    // Test helpers
    bool isInitialized() const { return initialized_; }
    uint32_t getShowCount() const { return showCount_; }
    uint8_t getPin() const { return pin_; }

private:
    uint8_t pin_;
    bool initialized_ = false;
    bool dirty_ = false;
    uint8_t brightness_ = 255;
    uint32_t showCount_ = 0;
    std::vector<RgbColor> pixels_;
};

// Common feature/method types (no-ops in emulation)
class NeoGrbFeature {};
class NeoRgbFeature {};
class NeoGrbwFeature {};
class NeoEsp32Rmt0800KbpsMethod {};
class NeoEsp32I2s1800KbpsMethod {};
class Neo800KbpsMethod {};
