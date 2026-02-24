#include <Arduino.h>
#include <esp32emu_board.h>

// DFRobot FireBeetle ESP32 blink example
// Built-in LED on GPIO 2

const int LED_PIN = 2;

void setup() {
    Serial.begin(115200);
    Serial.println("FireBeetle ESP32 Blink");

    esp32emu::Board::instance().setBoard(esp32emu::BoardType::FIREBEETLE_ESP32);
    esp32emu::Board::instance().printBoardInfo();
    esp32emu::Board::instance().printASCII();

    pinMode(LED_PIN, OUTPUT);
}

void loop() {
    digitalWrite(LED_PIN, HIGH);
    Serial.println("LED ON");
    delay(500);

    digitalWrite(LED_PIN, LOW);
    Serial.println("LED OFF");
    delay(500);
}
