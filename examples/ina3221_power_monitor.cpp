#include <Arduino.h>
#include <INA3221.h>

INA3221 ina(0x40);

const char* channelNames[] = {"5V Rail", "3.3V Rail", "12V Rail"};

void setup() {
    Serial.begin(115200);
    Serial.println("INA3221 Triple Power Monitor");

    if (!ina.begin()) {
        Serial.println("Failed to find INA3221!");
        while (1) delay(10);
    }

    for (int i = 0; i < 3; i++) {
        ina.setShuntResistance(i, 0.1f);
    }

    ina.setAveragingMode(4);  // 128 samples
    Serial.println("INA3221 initialized — 3 channels active");
}

void loop() {
    Serial.println("────────────────────────────────");
    for (uint8_t ch = 0; ch < 3; ch++) {
        if (!ina.isChannelEnabled(ch)) continue;

        float voltage = ina.getBusVoltage_V(ch);
        float current = ina.getCurrent_mA(ch);
        float power   = ina.getPower_mW(ch);

        Serial.print(channelNames[ch]);
        Serial.print(": ");
        Serial.print(voltage, 2);
        Serial.print("V  ");
        Serial.print(current, 1);
        Serial.print("mA  ");
        Serial.print(power, 1);
        Serial.println("mW");

        if (ina.isCriticalAlert(ch)) {
            Serial.print("  ⚠️ CRITICAL ALERT on ");
            Serial.println(channelNames[ch]);
        }
    }

    delay(2000);
}
