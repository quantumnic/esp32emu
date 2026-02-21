// TSL2591 high-dynamic-range lux sensor example
#include <Arduino.h>
#include "Adafruit_TSL2591.h"

Adafruit_TSL2591 tsl(2591);

void setup() {
    Serial.begin(115200);
    Serial.println("TSL2591 Lux Sensor");

    if (!tsl.begin()) {
        Serial.println("ERROR: TSL2591 not found!");
        return;
    }

    tsl.setGain(TSL2591_GAIN_MED);
    tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
    tsl.enable();

    // Inject test luminosity
    tsl.setLuminosity(600, 80);
    Serial.println("Sensor configured.");
}

void loop() {
    uint32_t full = tsl.getFullLuminosity();
    uint16_t ir = full >> 16;
    uint16_t vis = full & 0xFFFF;
    float lux = tsl.calculateLux(vis, ir);

    Serial.print("Visible: "); Serial.print(vis);
    Serial.print("  IR: "); Serial.print(ir);
    Serial.print("  Lux: "); Serial.println(lux);

    delay(1000);
}
