// WS2812FX LED effects demo — cycle through effects on a 30-LED strip
#include <WS2812FX.h>

#define LED_COUNT 30
#define LED_PIN   5

WS2812FX ws2812fx(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

uint8_t currentMode = 0;
unsigned long lastChange = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("WS2812FX Effects Demo");

    ws2812fx.init();
    ws2812fx.setBrightness(128);
    ws2812fx.setSpeed(1000);
    ws2812fx.setColor(0xFF5500);
    ws2812fx.setMode(FX_MODE_RAINBOW_CYCLE);
    ws2812fx.start();
}

void loop() {
    ws2812fx.service();

    // Cycle through modes every 5 seconds
    if (millis() - lastChange > 5000) {
        lastChange = millis();
        currentMode = (currentMode + 1) % ws2812fx.getModeCount();
        ws2812fx.setMode(currentMode);
        Serial.print("Mode: ");
        Serial.println(ws2812fx.getModeName(currentMode));
    }
    delay(10);
}
