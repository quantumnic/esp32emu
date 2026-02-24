// test_boards_new.cpp — Tests for M5StickC Plus, TTGO T-Display, XIAO ESP32-C6 boards
#include "esp32emu_board.h"
#include <cassert>
#include <cstdio>
#include <cstring>

using namespace esp32emu;

int main() {
    // M5StickC Plus
    {
        auto& cfg = getBoardConfig(BoardType::M5STICKC_PLUS);
        assert(strcmp(cfg.name, "M5StickC Plus") == 0);
        assert(strcmp(cfg.chip, "ESP32-PICO-D4") == 0);
        assert(cfg.has_wifi);
        assert(cfg.has_bluetooth);
        assert(cfg.cpu_freq_mhz == 240);
        assert(cfg.builtin_led == 10);

        // Parse name
        assert(parseBoardName("m5stickc-plus") == BoardType::M5STICKC_PLUS);
        assert(parseBoardName("m5stickc") == BoardType::M5STICKC_PLUS);
        assert(parseBoardName("m5stick") == BoardType::M5STICKC_PLUS);
    }

    // TTGO T-Display
    {
        auto& cfg = getBoardConfig(BoardType::TTGO_TDISPLAY);
        assert(strcmp(cfg.name, "TTGO T-Display") == 0);
        assert(cfg.has_wifi);
        assert(cfg.has_bluetooth);
        assert(cfg.cpu_freq_mhz == 240);
        assert(cfg.builtin_led == -1);

        assert(parseBoardName("ttgo-tdisplay") == BoardType::TTGO_TDISPLAY);
        assert(parseBoardName("t-display") == BoardType::TTGO_TDISPLAY);
        assert(parseBoardName("tdisplay") == BoardType::TTGO_TDISPLAY);
    }

    // XIAO ESP32-C6
    {
        auto& cfg = getBoardConfig(BoardType::SEEED_XIAO_ESP32C6);
        assert(strcmp(cfg.name, "XIAO ESP32-C6") == 0);
        assert(strcmp(cfg.chip, "ESP32-C6") == 0);
        assert(cfg.has_wifi);
        assert(cfg.has_bluetooth);
        assert(cfg.cpu_freq_mhz == 160);
        assert(cfg.builtin_led == 15);

        assert(parseBoardName("xiao-c6") == BoardType::SEEED_XIAO_ESP32C6);
        assert(parseBoardName("xiao-esp32c6") == BoardType::SEEED_XIAO_ESP32C6);
        assert(parseBoardName("xiao-esp32-c6") == BoardType::SEEED_XIAO_ESP32C6);
    }

    printf("test_boards_new: all assertions passed\n");
    return 0;
}
