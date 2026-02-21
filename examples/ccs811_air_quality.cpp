// esp32emu example — CCS811 Indoor Air Quality Monitor
#include <Arduino.h>
#include <CCS811.h>

CCS811 ccs;

void setup() {
    Serial.begin(115200);
    Serial.println("=== CCS811 Air Quality Monitor ===");

    auto status = ccs.beginWithStatus();
    if (status != CCS811::CCS811_Stat_SUCCESS) {
        Serial.printf("❌ CCS811 init failed: %d\n", status);
        return;
    }

    ccs.setDriveMode(1); // 1 second measurement
    ccs.setEnvironmentalData(50.0, 25.0);
    Serial.printf("Hardware ID: %s\n", ccs.getHardwareID().c_str());
    Serial.println("✅ Sensor ready (20 min warm-up recommended)");
}

void loop() {
    if (ccs.dataAvailable()) {
        ccs.readAlgorithmResults();
        uint16_t co2 = ccs.getCO2();
        uint16_t tvoc = ccs.getTVOC();

        Serial.printf("eCO2: %d ppm | TVOC: %d ppb", co2, tvoc);

        if (co2 > 1000) Serial.print(" ⚠️  Ventilate!");
        if (co2 > 2000) Serial.print(" 🔴 Poor air!");
        Serial.println();
    }

    if (ccs.checkForStatusError()) {
        Serial.printf("Sensor error: %s\n", ccs.getErrorRegister());
    }

    delay(1000);
}
