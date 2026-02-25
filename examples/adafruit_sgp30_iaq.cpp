// esp32emu example: Adafruit SGP30 Indoor Air Quality
// Measures eCO2 and TVOC with baseline save/restore
#include "Adafruit_SGP30.h"

Adafruit_SGP30 sgp;
uint32_t measureCount = 0;

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximate formula
    float absHum = 216.7f * ((humidity / 100.0f) * 6.112f *
        exp((17.62f * temperature) / (243.12f + temperature)) /
        (273.15f + temperature));
    return (uint32_t)(absHum * 1000.0f * 256.0f);
}

void setup() {
    Serial.begin(115200);
    Serial.println("SGP30 Indoor Air Quality Sensor");

    if (!sgp.begin()) {
        Serial.println("SGP30 not found!");
        while (1) delay(10);
    }

    Serial.printf("Serial #: %04X%04X%04X\n",
                  sgp.serialnumber[0], sgp.serialnumber[1], sgp.serialnumber[2]);

    // Optionally set humidity compensation
    sgp.setHumidity(getAbsoluteHumidity(22.1, 45.2));
}

void loop() {
    if (!sgp.IAQmeasure()) {
        Serial.println("Measurement failed!");
        return;
    }

    Serial.printf("TVOC: %d ppb  |  eCO2: %d ppm\n", sgp.TVOC, sgp.eCO2);

    if (!sgp.IAQmeasureRaw()) {
        Serial.println("Raw measurement failed!");
        return;
    }
    Serial.printf("Raw H2: %d  |  Raw Ethanol: %d\n", sgp.rawH2, sgp.rawEthanol);

    // Save baseline every 30 measurements
    measureCount++;
    if (measureCount % 30 == 0) {
        uint16_t eco2_base, tvoc_base;
        if (sgp.getIAQBaseline(&eco2_base, &tvoc_base)) {
            Serial.printf("Baseline - eCO2: 0x%04X  TVOC: 0x%04X\n", eco2_base, tvoc_base);
        }
    }

    delay(1000);
}
