#include "esp32emu_board.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // QT Py ESP32-S2
    auto qtpy = getBoardConfig(esp32emu::BoardType::QTPY_ESP32S2);
    assert(strcmp(qtpy.name, "QT Py ESP32-S2") == 0);
    assert(strcmp(qtpy.chip, "ESP32-S2") == 0);
    assert(qtpy.has_wifi);
    assert(!qtpy.has_bluetooth); // S2 has no BT
    assert(qtpy.builtin_led == 39); // NeoPixel power pin
    assert(qtpy.cpu_freq_mhz == 240);

    // Parse name
    assert(esp32emu::parseBoardName("qtpy") == esp32emu::BoardType::QTPY_ESP32S2);
    assert(esp32emu::parseBoardName("qtpy-s2") == esp32emu::BoardType::QTPY_ESP32S2);

    // SparkFun Thing Plus ESP32-S3
    auto tp = getBoardConfig(esp32emu::BoardType::THINGPLUS_ESP32S3);
    assert(strcmp(tp.name, "Thing Plus ESP32-S3") == 0);
    assert(strcmp(tp.chip, "ESP32-S3") == 0);
    assert(tp.has_wifi);
    assert(tp.has_bluetooth);
    assert(tp.builtin_led == 13);
    assert(tp.flash_bytes == 16777216); // 16MB

    // Parse name
    assert(esp32emu::parseBoardName("thingplus") == esp32emu::BoardType::THINGPLUS_ESP32S3);
    assert(esp32emu::parseBoardName("sparkfun-thingplus") == esp32emu::BoardType::THINGPLUS_ESP32S3);

    printf("test_board_qtpy_thingplus: all assertions passed\n");
    return 0;
}
