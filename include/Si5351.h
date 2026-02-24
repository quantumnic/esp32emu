#pragma once
// esp32emu — Si5351 Programmable Clock Generator mock
// Silicon Labs Si5351A/B/C I2C triple-output clock generator, 8kHz–160MHz.

#include <cstdint>
#include <cstdio>

enum si5351_clock {
    SI5351_CLK0 = 0,
    SI5351_CLK1 = 1,
    SI5351_CLK2 = 2
};

enum si5351_pll {
    SI5351_PLLA = 0,
    SI5351_PLLB = 1
};

enum si5351_drive {
    SI5351_DRIVE_2MA = 0,
    SI5351_DRIVE_4MA = 1,
    SI5351_DRIVE_6MA = 2,
    SI5351_DRIVE_8MA = 3
};

class Si5351 {
public:
    Si5351(uint8_t addr = 0x60) : addr_(addr) {}

    bool init(uint8_t xtal_load = 0, uint32_t ref_freq = 25000000) {
        xtal_load_ = xtal_load;
        ref_freq_ = ref_freq;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] Si5351 initialized at 0x%02X, ref=%uHz\n", addr_, ref_freq_);
        return true;
    }

    void set_freq(uint64_t freq_hz_x100, si5351_clock clk) {
        if (clk <= SI5351_CLK2) {
            freq_[clk] = freq_hz_x100;
            fprintf(stderr, "[esp32emu] Si5351 CLK%d = %llu.%02llu Hz\n",
                    clk, (unsigned long long)(freq_hz_x100 / 100),
                    (unsigned long long)(freq_hz_x100 % 100));
        }
    }

    uint64_t get_freq(si5351_clock clk) const {
        return (clk <= SI5351_CLK2) ? freq_[clk] : 0;
    }

    void output_enable(si5351_clock clk, bool enable) {
        if (clk <= SI5351_CLK2) enabled_[clk] = enable;
    }

    bool is_output_enabled(si5351_clock clk) const {
        return (clk <= SI5351_CLK2) ? enabled_[clk] : false;
    }

    void drive_strength(si5351_clock clk, si5351_drive drive) {
        if (clk <= SI5351_CLK2) drive_[clk] = drive;
    }

    si5351_drive get_drive_strength(si5351_clock clk) const {
        return (clk <= SI5351_CLK2) ? drive_[clk] : SI5351_DRIVE_2MA;
    }

    void set_correction(int32_t corr) { correction_ = corr; }
    int32_t get_correction() const { return correction_; }

    void set_pll(uint64_t freq_hz_x100, si5351_pll pll) {
        if (pll <= SI5351_PLLB) pll_freq_[pll] = freq_hz_x100;
    }

    bool isInitialized() const { return initialized_; }
    uint8_t getAddress() const { return addr_; }

private:
    uint8_t addr_;
    bool initialized_ = false;
    uint8_t xtal_load_ = 0;
    uint32_t ref_freq_ = 25000000;
    int32_t correction_ = 0;
    uint64_t freq_[3] = {0, 0, 0};
    bool enabled_[3] = {true, true, true};
    si5351_drive drive_[3] = {SI5351_DRIVE_2MA, SI5351_DRIVE_2MA, SI5351_DRIVE_2MA};
    uint64_t pll_freq_[2] = {0, 0};
};
