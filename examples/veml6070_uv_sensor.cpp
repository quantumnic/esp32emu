// esp32emu example — Adafruit VEML6070 UV Light Sensor
#include <Arduino.h>
#include <Adafruit_VEML6070.h>

Adafruit_VEML6070 uv;

void setup() {
    Serial.begin(115200);
    Serial.println("=== VEML6070 UV Light Sensor Demo ===");

    uv.begin(VEML6070_1_T);
    Serial.println("✅ VEML6070 initialized");

    // Simulate UV readings
    uv.test_setUV(320);
}

void loop() {
    uint16_t uvRaw = uv.readUV();
    Serial.printf("UV raw: %d", uvRaw);

    // Simple UV index estimation
    const char* risk;
    if (uvRaw < 227)      risk = "LOW";
    else if (uvRaw < 318)  risk = "MODERATE";
    else if (uvRaw < 408)  risk = "HIGH";
    else if (uvRaw < 503)  risk = "VERY HIGH";
    else                    risk = "EXTREME";

    Serial.printf(" → Risk: %s\n", risk);
    delay(1000);
}
