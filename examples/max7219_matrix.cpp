// MAX7219 LED matrix example
#include "Arduino.h"
#include "MAX7219.h"

MAX7219 matrix(23, 5, 18, 1);

void setup() {
    Serial.begin(115200);
    matrix.begin();
    matrix.shutdown(0, false);
    matrix.setIntensity(0, 8);
    matrix.clearDisplay(0);

    // Draw a smiley face
    matrix.setRow(0, 0, 0b00111100);
    matrix.setRow(0, 1, 0b01000010);
    matrix.setRow(0, 2, 0b10100101);
    matrix.setRow(0, 3, 0b10000001);
    matrix.setRow(0, 4, 0b10100101);
    matrix.setRow(0, 5, 0b10011001);
    matrix.setRow(0, 6, 0b01000010);
    matrix.setRow(0, 7, 0b00111100);

    Serial.println("Smiley displayed!");
}

void loop() {
    delay(1000);
}
