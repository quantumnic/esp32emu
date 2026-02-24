// M5StickC Plus board demo — LED blink with board info
#include <Arduino.h>
#include "esp32emu_board.h"

using namespace esp32emu;

void setup() {
    Serial.begin(115200);

    auto& board = getBoardConfig(BoardType::M5STICKC_PLUS);
    Serial.print("Board: ");
    Serial.println(board.name);
    Serial.print("Chip: ");
    Serial.println(board.chip);
    Serial.print("CPU: ");
    Serial.print(board.cpu_freq_mhz);
    Serial.println(" MHz");
    Serial.print("WiFi: ");
    Serial.println(board.has_wifi ? "Yes" : "No");
    Serial.print("Bluetooth: ");
    Serial.println(board.has_bluetooth ? "Yes" : "No");

    if (board.builtin_led >= 0) {
        pinMode(board.builtin_led, OUTPUT);
    }
}

void loop() {
    auto& board = getBoardConfig(BoardType::M5STICKC_PLUS);
    if (board.builtin_led >= 0) {
        digitalWrite(board.builtin_led, HIGH);
        delay(500);
        digitalWrite(board.builtin_led, LOW);
        delay(500);
    }
}
