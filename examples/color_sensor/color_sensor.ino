// TCS34725 Color Sensor — detect RGB colors and color temperature
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>

Adafruit_TCS34725 tcs(0xEB, 0x01);  // 50ms integration, 1x gain

void setup() {
    Serial.begin(115200);
    Wire.begin();
    Serial.println("TCS34725 Color Sensor");

    if (!tcs.begin()) {
        Serial.println("TCS34725 not found!");
        while (true) delay(1000);
    }

    tcs.enable();
    Serial.println("Color sensor ready");
}

void loop() {
    uint16_t r, g, b, c;
    tcs.getRawData(&r, &g, &b, &c);

    float rf, gf, bf;
    tcs.getRGB(&rf, &gf, &bf);

    uint16_t colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
    uint16_t lux = tcs.calculateLux(r, g, b);

    Serial.print("R: "); Serial.print(r);
    Serial.print("  G: "); Serial.print(g);
    Serial.print("  B: "); Serial.print(b);
    Serial.print("  C: "); Serial.println(c);

    char buf[64];
    snprintf(buf, sizeof(buf), "RGB: (%.0f, %.0f, %.0f)", rf, gf, bf);
    Serial.println(buf);
    snprintf(buf, sizeof(buf), "Color Temp: %d K  Lux: %d", colorTemp, lux);
    Serial.println(buf);
    Serial.println("---");

    delay(1000);
}
