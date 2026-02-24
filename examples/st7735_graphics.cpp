// ST7735 TFT graphics demo — draw shapes and text
#include <Arduino.h>
#include <Adafruit_ST7735.h>

#define TFT_CS   5
#define TFT_DC   4
#define TFT_RST  16

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

void setup() {
    Serial.begin(115200);
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);
    tft.fillScreen(ST77XX_BLACK);

    // Draw colored rectangles
    tft.fillRect(0, 0, 40, 40, ST77XX_RED);
    tft.fillRect(40, 0, 40, 40, ST77XX_GREEN);
    tft.fillRect(80, 0, 40, 40, ST77XX_BLUE);

    // Draw lines
    tft.drawFastHLine(0, 50, 128, ST77XX_WHITE);
    tft.drawFastVLine(64, 50, 60, ST77XX_YELLOW);

    // Draw pixels in a pattern
    for (int x = 0; x < 128; x += 4) {
        tft.drawPixel(x, 120, ST77XX_CYAN);
    }

    Serial.println("ST7735 graphics demo complete");
}

void loop() {
    delay(1000);
}
