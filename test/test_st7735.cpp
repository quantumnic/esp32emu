// test_st7735.cpp — Adafruit ST7735 TFT display tests
#include "Adafruit_ST7735.h"
#include <cassert>
#include <cstdio>

int main() {
    // Test initR with default (green tab)
    {
        Adafruit_ST7735 tft(5, 4);
        tft.initR(INITR_GREENTAB);
        assert(tft.width() == 128);
        assert(tft.height() == 160);
        assert(tft.getTabType() == INITR_GREENTAB);
    }

    // Test initR with mini 160x80
    {
        Adafruit_ST7735 tft(5, 4);
        tft.initR(INITR_MINI160x80);
        assert(tft.width() == 80);
        assert(tft.height() == 160);
    }

    // Test initR with 144 green tab (128x128)
    {
        Adafruit_ST7735 tft(5, 4, 3);
        tft.initR(INITR_144GREENTAB);
        assert(tft.width() == 128);
        assert(tft.height() == 128);
    }

    // Test initB
    {
        Adafruit_ST7735 tft(5, 4);
        tft.initB();
        assert(tft.width() == 128);
        assert(tft.height() == 160);
    }

    // Test pixel operations
    {
        Adafruit_ST7735 tft(5, 4);
        tft.initR(INITR_REDTAB);
        tft.fillScreen(ST77XX_BLACK);
        assert(tft.getPixel(0, 0) == ST77XX_BLACK);

        tft.drawPixel(10, 20, ST77XX_RED);
        assert(tft.getPixel(10, 20) == ST77XX_RED);
        assert(tft.getPixel(11, 20) == ST77XX_BLACK);
    }

    // Test fillRect
    {
        Adafruit_ST7735 tft(5, 4);
        tft.initR();
        tft.fillScreen(0);
        tft.fillRect(10, 10, 5, 5, ST77XX_BLUE);
        assert(tft.getPixel(12, 12) == ST77XX_BLUE);
        assert(tft.getPixel(9, 9) == 0);
    }

    // Test rotation
    {
        Adafruit_ST7735 tft(5, 4);
        tft.initR();
        tft.setRotation(2);
        assert(tft.getRotation() == 2);
        tft.setRotation(5); // wraps to 1
        assert(tft.getRotation() == 1);
    }

    // Test invert and sleep
    {
        Adafruit_ST7735 tft(5, 4);
        tft.initR();
        assert(!tft.getInvert());
        tft.invertDisplay(true);
        assert(tft.getInvert());
        assert(!tft.isSleeping());
        tft.enableSleep(true);
        assert(tft.isSleeping());
    }

    // Test color565
    {
        uint16_t red = Adafruit_ST7735::color565(255, 0, 0);
        assert(red == 0xF800);
        uint16_t white = Adafruit_ST7735::color565(255, 255, 255);
        assert(white == 0xFFFF);
    }

    // Test SPI speed
    {
        Adafruit_ST7735 tft(5, 4);
        tft.initR();
        assert(tft.getSPISpeed() == 40000000);
        tft.setSPISpeed(20000000);
        assert(tft.getSPISpeed() == 20000000);
    }

    // Test out-of-bounds pixel
    {
        Adafruit_ST7735 tft(5, 4);
        tft.initR();
        tft.drawPixel(-1, -1, 0xFFFF); // should not crash
        tft.drawPixel(200, 200, 0xFFFF); // out of bounds
        assert(tft.getPixel(-1, -1) == 0);
    }

    printf("test_st7735: all assertions passed\n");
    return 0;
}
