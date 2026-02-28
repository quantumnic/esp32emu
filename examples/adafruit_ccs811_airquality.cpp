// esp32emu example — Adafruit CCS811 Air Quality Sensor (eCO2 + TVOC)
#include <Arduino.h>
#include <Adafruit_CCS811.h>

Adafruit_CCS811 ccs;

void setup() {
    Serial.begin(115200);
    Serial.println("Adafruit CCS811 Air Quality Sensor");

    if (!ccs.begin()) {
        Serial.println("Failed to start CCS811!");
        while (1) delay(10);
    }
    Serial.println("CCS811 initialized");

    // Set environmental compensation
    ccs.setEnvironmentalData(55.0, 23.0);

    // Test: simulate readings
    ccs.setECO2(850);
    ccs.setTVOC(35);
}

void loop() {
    if (ccs.available()) {
        if (ccs.readData() == 0) {
            uint16_t eco2 = ccs.geteCO2();
            uint16_t tvoc = ccs.getTVOC();

            Serial.print("eCO2: ");
            Serial.print(eco2);
            Serial.print(" ppm  TVOC: ");
            Serial.print(tvoc);
            Serial.print(" ppb  Temp: ");
            Serial.print(ccs.calculateTemperature());
            Serial.println(" °C");

            // Air quality assessment
            if (eco2 < 600) Serial.println("  -> Excellent air quality");
            else if (eco2 < 1000) Serial.println("  -> Good air quality");
            else if (eco2 < 1500) Serial.println("  -> Moderate - ventilate");
            else Serial.println("  -> Poor - open windows!");
        }
    }

    delay(1000);
}
