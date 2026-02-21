// Example: ILI9341 TFT display with graphics primitives
#include <Arduino.h>
#include "Adafruit_ILI9341.h"
#include <cstdio>

#define TFT_CS   5
#define TFT_DC   4
#define TFT_RST  16

Adafruit_ILI9341 tft(TFT_CS, TFT_DC, TFT_RST);

void setup() {
    tft.begin();
    tft.setRotation(1); // landscape
    tft.fillScreen(ILI9341_BLACK);

    // Draw colorful rectangles
    tft.fillRect(10, 10, 80, 60, ILI9341_RED);
    tft.fillRect(100, 10, 80, 60, ILI9341_GREEN);
    tft.fillRect(190, 10, 80, 60, ILI9341_BLUE);

    // Draw circles
    tft.fillCircle(50, 150, 30, ILI9341_YELLOW);
    tft.drawCircle(150, 150, 30, ILI9341_CYAN);

    // Text
    tft.setCursor(10, 200);
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.println("esp32emu ILI9341");
    tft.setTextSize(1);
    tft.print("320x240 TFT Display Mock");

    printf("Graphics demo drawn!\n");
}

void loop() {
    delay(1000);
}
