#include "../include/esp32emu_board.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    using namespace esp32emu;

    // Test all board types exist and have valid configs
    auto& esp32 = getBoardConfig(BoardType::ESP32);
    assert(strcmp(esp32.name, "ESP32") == 0);
    assert(esp32.has_wifi && esp32.has_bluetooth);

    auto& c6 = getBoardConfig(BoardType::ESP32_C6);
    assert(strcmp(c6.name, "ESP32-C6") == 0);
    assert(c6.has_wifi && c6.has_bluetooth);
    assert(c6.cpu_freq_mhz == 160);
    assert(c6.digital_pins == 31);

    auto& pico = getBoardConfig(BoardType::RP2040);
    assert(strcmp(pico.name, "RP2040 Pico") == 0);
    assert(!pico.has_wifi && !pico.has_bluetooth);
    assert(pico.cpu_freq_mhz == 133);
    assert(pico.ram_bytes == 264000);
    assert(pico.builtin_led == 25);

    // Test board name parsing
    assert(parseBoardName("esp32-c6") == BoardType::ESP32_C6);
    assert(parseBoardName("esp32c6") == BoardType::ESP32_C6);
    assert(parseBoardName("pico") == BoardType::RP2040);
    assert(parseBoardName("rp2040") == BoardType::RP2040);
    assert(parseBoardName("uno") == BoardType::ARDUINO_UNO);
    assert(parseBoardName("unknown") == BoardType::ESP32); // default

    // Test Board singleton
    Board::instance().setBoard(BoardType::RP2040);
    assert(Board::instance().config().builtin_led == 25);
    assert(Board::instance().A0_base() == 26);

    Board::instance().setBoard(BoardType::ESP32_C6);
    assert(Board::instance().config().digital_pins == 31);
    assert(!Board::instance().checkWiFi() == false); // C6 has WiFi, so checkWiFi returns true

    // Reset
    Board::instance().setBoard(BoardType::ESP32);

    printf("test_boards: all assertions passed\n");
    return 0;
}
