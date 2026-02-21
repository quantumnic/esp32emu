#include "NeoPixelBus.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    // Test 1: Basic init and pixel operations
    {
        NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(10, 5);
        strip.Begin();
        assert(strip.isInitialized());
        assert(strip.PixelCount() == 10);
        assert(strip.getPin() == 5);

        strip.SetPixelColor(0, RgbColor(255, 0, 0));
        strip.SetPixelColor(9, RgbColor(0, 0, 255));
        assert(strip.IsDirty());

        auto c0 = strip.GetPixelColor(0);
        assert(c0.R == 255 && c0.G == 0 && c0.B == 0);
        auto c9 = strip.GetPixelColor(9);
        assert(c9.R == 0 && c9.G == 0 && c9.B == 255);

        strip.Show();
        assert(!strip.IsDirty());
        assert(strip.getShowCount() == 1);
    }

    // Test 2: ClearTo
    {
        NeoPixelBus<> strip(5);
        strip.Begin();
        strip.ClearTo(RgbColor(100, 200, 50));
        for (uint16_t i = 0; i < 5; i++) {
            auto c = strip.GetPixelColor(i);
            assert(c.R == 100 && c.G == 200 && c.B == 50);
        }
    }

    // Test 3: Out of bounds
    {
        NeoPixelBus<> strip(3);
        strip.Begin();
        strip.SetPixelColor(100, RgbColor(255, 255, 255)); // should not crash
        auto c = strip.GetPixelColor(100);
        assert(c.R == 0 && c.G == 0 && c.B == 0); // returns black
    }

    // Test 4: RotateLeft/Right
    {
        NeoPixelBus<> strip(4);
        strip.Begin();
        strip.SetPixelColor(0, RgbColor(1, 0, 0));
        strip.SetPixelColor(1, RgbColor(2, 0, 0));
        strip.SetPixelColor(2, RgbColor(3, 0, 0));
        strip.SetPixelColor(3, RgbColor(4, 0, 0));

        strip.RotateLeft(1);
        assert(strip.GetPixelColor(0).R == 2);
        assert(strip.GetPixelColor(3).R == 1);

        strip.RotateRight(1);
        assert(strip.GetPixelColor(0).R == 1);
        assert(strip.GetPixelColor(3).R == 4);
    }

    // Test 5: RgbColor::LinearBlend
    {
        RgbColor a(0, 0, 0), b(100, 200, 50);
        auto mid = RgbColor::LinearBlend(a, b, 0.5f);
        assert(mid.R == 50 && mid.G == 100 && mid.B == 25);

        auto start = RgbColor::LinearBlend(a, b, 0.0f);
        assert(start == a);
        auto end = RgbColor::LinearBlend(a, b, 1.0f);
        assert(end == b);
    }

    // Test 6: HslColor from RgbColor
    {
        HslColor hsl(RgbColor(255, 0, 0)); // pure red
        assert(fabs(hsl.H - 0.0f) < 0.01f);
        assert(fabs(hsl.S - 1.0f) < 0.01f);
        assert(fabs(hsl.L - 0.5f) < 0.01f);
    }

    // Test 7: RgbwColor
    {
        RgbwColor c(10, 20, 30, 40);
        assert(c.R == 10 && c.G == 20 && c.B == 30 && c.W == 40);
        RgbwColor c2(10, 20, 30, 40);
        assert(c == c2);
    }

    // Test 8: Brightness
    {
        NeoPixelBus<> strip(1);
        strip.Begin();
        strip.SetBrightness(128);
        assert(strip.GetBrightness() == 128);
    }

    // Test 9: ShiftLeft
    {
        NeoPixelBus<> strip(3);
        strip.Begin();
        strip.SetPixelColor(0, RgbColor(1, 0, 0));
        strip.SetPixelColor(1, RgbColor(2, 0, 0));
        strip.SetPixelColor(2, RgbColor(3, 0, 0));
        strip.ShiftLeft(1);
        assert(strip.GetPixelColor(0).R == 2);
        assert(strip.GetPixelColor(1).R == 3);
        assert(strip.GetPixelColor(2).R == 0); // shifted in black
    }

    printf("test_neopixelbus: all assertions passed\n");
    return 0;
}
