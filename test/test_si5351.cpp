#include "Si5351.h"
#include <cassert>
#include <cstdio>

int main() {
    // Test 1: Init
    {
        Si5351 clk;
        assert(!clk.isInitialized());
        assert(clk.init());
        assert(clk.isInitialized());
    }

    // Test 2: Set frequency
    {
        Si5351 clk;
        clk.init();
        clk.set_freq(1000000000ULL, SI5351_CLK0); // 10 MHz (in centi-Hz)
        assert(clk.get_freq(SI5351_CLK0) == 1000000000ULL);
    }

    // Test 3: Multiple clocks
    {
        Si5351 clk;
        clk.init();
        clk.set_freq(1000000000ULL, SI5351_CLK0); // 10 MHz
        clk.set_freq(2500000000ULL, SI5351_CLK1); // 25 MHz
        clk.set_freq(14400000000ULL, SI5351_CLK2); // 144 MHz
        assert(clk.get_freq(SI5351_CLK0) == 1000000000ULL);
        assert(clk.get_freq(SI5351_CLK1) == 2500000000ULL);
        assert(clk.get_freq(SI5351_CLK2) == 14400000000ULL);
    }

    // Test 4: Output enable/disable
    {
        Si5351 clk;
        clk.init();
        assert(clk.is_output_enabled(SI5351_CLK0));
        clk.output_enable(SI5351_CLK0, false);
        assert(!clk.is_output_enabled(SI5351_CLK0));
        clk.output_enable(SI5351_CLK0, true);
        assert(clk.is_output_enabled(SI5351_CLK0));
    }

    // Test 5: Drive strength
    {
        Si5351 clk;
        clk.init();
        clk.drive_strength(SI5351_CLK0, SI5351_DRIVE_8MA);
        assert(clk.get_drive_strength(SI5351_CLK0) == SI5351_DRIVE_8MA);
        clk.drive_strength(SI5351_CLK1, SI5351_DRIVE_4MA);
        assert(clk.get_drive_strength(SI5351_CLK1) == SI5351_DRIVE_4MA);
    }

    // Test 6: Correction
    {
        Si5351 clk;
        clk.init();
        clk.set_correction(15000);
        assert(clk.get_correction() == 15000);
        clk.set_correction(-5000);
        assert(clk.get_correction() == -5000);
    }

    // Test 7: PLL config
    {
        Si5351 clk;
        clk.init();
        clk.set_pll(90000000000ULL, SI5351_PLLA); // 900 MHz
        // No crash = success (PLL freq is internal)
    }

    // Test 8: Custom address
    {
        Si5351 clk(0x61);
        clk.init();
        assert(clk.getAddress() == 0x61);
    }

    printf("test_si5351: all assertions passed\n");
    return 0;
}
