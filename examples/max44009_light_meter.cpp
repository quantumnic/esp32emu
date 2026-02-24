#include <Arduino.h>
#include <MAX44009.h>

MAX44009 light(0x4A);

void setup() {
    Serial.begin(115200);
    Serial.println("MAX44009 Ambient Light Sensor");

    light.begin();
    light.setContinuousMode(true);
    light.setHighThreshold(50000.0f);
    light.setLowThreshold(1.0f);
    light.enableInterrupt(true);

    Serial.println("Thresholds: 1 - 50000 lux");
}

void loop() {
    float lux = light.getLux();

    Serial.print("Light: ");
    Serial.print(lux, 1);
    Serial.print(" lux");

    if (lux < 10) {
        Serial.print("  🌙 Dark");
    } else if (lux < 500) {
        Serial.print("  🏠 Indoor");
    } else if (lux < 10000) {
        Serial.print("  ⛅ Overcast");
    } else {
        Serial.print("  ☀️ Bright sunlight");
    }

    if (light.getInterruptStatus()) {
        Serial.print("  [ALERT]");
    }

    Serial.println();
    delay(500);
}
