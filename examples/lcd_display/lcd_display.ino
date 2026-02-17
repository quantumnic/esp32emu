// Text auf simuliertem LCD anzeigen
// esp32emu run --board uno examples/lcd_display/lcd_display.ino
#include "Arduino.h"
#include "LiquidCrystal.h"

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

unsigned long lastUpdate = 0;
int screen = 0;

void setup() {
    Serial.begin(115200);
    lcd.begin(16, 2);
    Serial.println("📺 LCD Display Demo\n");
    
    lcd.setCursor(0, 0);
    lcd.print("esp32emu v0.2.0");
    lcd.setCursor(0, 1);
    lcd.print("LCD Demo!");
}

void loop() {
    if (millis() - lastUpdate >= 3000) {
        lastUpdate = millis();
        screen = (screen + 1) % 4;
        lcd.clear();

        switch (screen) {
            case 0:
                lcd.setCursor(0, 0);
                lcd.print("Hello World!");
                lcd.setCursor(0, 1);
                lcd.print("Arduino + LCD");
                break;
            case 1:
                lcd.setCursor(0, 0);
                lcd.print("Uptime:");
                lcd.setCursor(0, 1);
                lcd.print((long)(millis() / 1000));
                lcd.print(" seconds");
                break;
            case 2:
                lcd.setCursor(0, 0);
                lcd.print("Temp: 23.5 C");
                lcd.setCursor(0, 1);
                lcd.print("Humidity: 61%");
                break;
            case 3:
                lcd.setCursor(0, 0);
                lcd.print("esp32emu rocks!");
                lcd.setCursor(0, 1);
                lcd.print("================");
                break;
        }
    }
    delay(100);
}
