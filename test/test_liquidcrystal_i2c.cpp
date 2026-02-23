#include "LiquidCrystal_I2C.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Init
    {
        LiquidCrystal_I2C lcd(0x27, 16, 2);
        lcd.begin();
        assert(lcd.isInitialized());
        assert(lcd.getCols() == 16);
        assert(lcd.getRows() == 2);
    }

    // Print and read back
    {
        LiquidCrystal_I2C lcd(0x27, 16, 2);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("Hello World!");
        assert(lcd.getChar(0, 0) == 'H');
        assert(lcd.getChar(5, 0) == ' ');
        assert(lcd.getChar(6, 0) == 'W');

        char row[20];
        lcd.getRow(0, row, sizeof(row));
        assert(strncmp(row, "Hello World!    ", 16) == 0);
    }

    // Multi-row
    {
        LiquidCrystal_I2C lcd(0x27, 20, 4);
        lcd.begin();
        lcd.setCursor(0, 0);
        lcd.print("Line 1");
        lcd.setCursor(0, 1);
        lcd.print("Line 2");
        lcd.setCursor(0, 2);
        lcd.print("Line 3");

        assert(lcd.getChar(0, 0) == 'L');
        assert(lcd.getChar(5, 1) == '2');
        assert(lcd.getChar(5, 2) == '3');
    }

    // Clear
    {
        LiquidCrystal_I2C lcd(0x27, 16, 2);
        lcd.begin();
        lcd.print("test");
        lcd.clear();
        assert(lcd.getChar(0, 0) == ' ');
        assert(lcd.getCursorCol() == 0);
        assert(lcd.getCursorRow() == 0);
    }

    // Backlight
    {
        LiquidCrystal_I2C lcd(0x27, 16, 2);
        lcd.begin();
        assert(lcd.isBacklight());
        lcd.noBacklight();
        assert(!lcd.isBacklight());
        lcd.backlight();
        assert(lcd.isBacklight());
    }

    // Print int/float
    {
        LiquidCrystal_I2C lcd(0x27, 16, 2);
        lcd.begin();
        lcd.print(42);
        assert(lcd.getChar(0, 0) == '4');
        assert(lcd.getChar(1, 0) == '2');
    }

    // Custom char
    {
        LiquidCrystal_I2C lcd(0x27, 16, 2);
        lcd.begin();
        uint8_t heart[8] = {0,10,31,31,14,4,0,0};
        lcd.createChar(0, heart); // no crash
    }

    printf("test_liquidcrystal_i2c: all assertions passed\n");
    return 0;
}
