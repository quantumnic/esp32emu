#include <Arduino.h>
#include <ADS1220.h>

ADS1220 adc;

void setup() {
    Serial.begin(115200);
    Serial.println("ADS1220 Load Cell / Precision ADC");

    adc.begin(5, 4); // CS=5, DRDY=4
    adc.setGain(7);    // 128x for load cell
    adc.setDataRate(3); // 175 SPS
    adc.setMux(0x01);   // AIN0-AIN1 differential
    adc.setContinuousMode(true);

    Serial.println("ADC configured for load cell reading.");
}

void loop() {
    if (adc.isDataReady()) {
        int32_t raw = adc.readRaw();
        float voltage = adc.readVoltage();

        Serial.print("Raw: ");
        Serial.print(raw);
        Serial.print("  Voltage: ");
        Serial.print(voltage * 1000.0f, 3);
        Serial.print(" mV");

        // Simple weight estimation (example calibration)
        float weight_g = raw * 0.001f;
        Serial.print("  Weight: ");
        Serial.print(weight_g, 1);
        Serial.println(" g");
    }

    delay(100);
}
