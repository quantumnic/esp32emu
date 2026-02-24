// esp32emu example — Adafruit ENS160 Air Quality Sensor
#include <Arduino.h>
#include <Adafruit_ENS160.h>

Adafruit_ENS160 ens;

void setup() {
    Serial.begin(115200);
    Serial.println("=== ENS160 Air Quality Sensor Demo ===");

    if (!ens.begin()) {
        Serial.println("❌ ENS160 not found!");
        return;
    }
    Serial.printf("✅ ENS160 found — Firmware: 0x%04X\n", ens.getFirmwareVersion());

    ens.setMode(2); // Standard operating mode
    ens.setTempCompensation(22.0f);
    ens.setRHCompensation(55.0f);

    // Simulate sensor readings
    ens.test_setAQI(2);
    ens.test_setTVOC(150);
    ens.test_setECO2(650);
}

void loop() {
    if (ens.dataReady()) {
        Serial.printf("AQI: %d | TVOC: %d ppb | eCO2: %d ppm\n",
                      ens.getAQI(), ens.getTVOC(), ens.getECO2());

        const char* labels[] = {"", "Excellent", "Good", "Moderate", "Poor", "Unhealthy"};
        uint8_t aqi = ens.getAQI();
        if (aqi >= 1 && aqi <= 5) {
            Serial.printf("Air quality: %s\n", labels[aqi]);
        }
    }
    delay(1000);
}
