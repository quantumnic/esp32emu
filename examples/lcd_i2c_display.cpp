// esp32emu example: I2C LCD Display
// Shows temperature and humidity on a 16x2 I2C LCD.

#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int counter = 0;

void setup() {
    Serial.begin(115200);
    Wire.begin();

    lcd.begin();
    lcd.backlight();

    // Custom degree symbol
    uint8_t degree[8] = {0x06, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00, 0x00};
    lcd.createChar(0, degree);

    lcd.setCursor(0, 0);
    lcd.print("ESP32 Weather");
    lcd.setCursor(0, 1);
    lcd.print("Starting...");

    Serial.println("LCD initialized");
    delay(2000);
}

void loop() {
    float temp = 22.5f + (counter % 10) * 0.1f;
    float hum = 45.0f + (counter % 20) * 0.5f;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temp, 1);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("Hum:  ");
    lcd.print(hum, 1);
    lcd.print("%");

    Serial.printf("T=%.1fC H=%.1f%%\n", temp, hum);
    counter++;
    delay(2000);
}
