#include <Arduino.h>
#include "esp32emu_board.h"
#include <WiFi.h>

using namespace esp32emu;

void setup() {
    Serial.begin(115200);
    Board::instance().setBoard(BoardType::M5STACK_CORE2);
    const auto& cfg = Board::instance().config();

    Serial.printf("Board: %s\n", cfg.name);
    Serial.printf("Chip: %s @ %d MHz\n", cfg.chip, cfg.cpu_freq_mhz);
    Serial.printf("RAM: %d KB, Flash: %d MB\n", cfg.ram_bytes / 1024, cfg.flash_bytes / (1024*1024));
    Serial.printf("WiFi: %s, BT: %s\n", cfg.has_wifi ? "yes" : "no", cfg.has_bluetooth ? "yes" : "no");
    Serial.printf("Digital pins: %d, Analog pins: %d\n", cfg.digital_pins, cfg.analog_pins);

    WiFi.begin("TestNetwork", "password");
    Serial.println("M5Stack Core2 initialized");
}

void loop() {
    Serial.println("M5Stack Core2 running...");
    delay(2000);
}
