// Adafruit ADS1115 16-bit ADC — multi-channel voltage reading
#include <Arduino.h>
#include <Adafruit_ADS1X15.h>

Adafruit_ADS1115 ads;

void setup() {
    Serial.begin(115200);
    if (!ads.begin(0x48)) {
        Serial.println("Failed to init ADS1115!");
        while (1);
    }
    ads.setGain(GAIN_ONE);  // +/- 4.096V range
    Serial.println("ADS1115 ready");
}

void loop() {
    for (int ch = 0; ch < 4; ch++) {
        int16_t raw = ads.readADC_SingleEnded(ch);
        float voltage = ads.computeVolts(raw);
        Serial.print("Ch"); Serial.print(ch);
        Serial.print(": "); Serial.print(voltage, 4);
        Serial.print("V (raw="); Serial.print(raw);
        Serial.println(")");
    }

    int16_t diff01 = ads.readADC_Differential_0_1();
    Serial.print("Diff 0-1: "); Serial.print(ads.computeVolts(diff01), 4);
    Serial.println("V");
    Serial.println("---");
    delay(1000);
}
