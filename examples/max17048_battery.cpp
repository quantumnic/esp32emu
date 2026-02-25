// esp32emu example — MAX17048 battery fuel gauge: monitor LiPo battery
#include <Arduino.h>
#include <Adafruit_MAX1704X.h>

Adafruit_MAX17048 gauge;

void setup() {
    Serial.begin(115200);
    if (!gauge.begin()) {
        Serial.println("MAX17048 not found!");
        while (1) delay(10);
    }
    Serial.println("MAX17048 battery monitor ready");
    gauge.quickStart();
}

void loop() {
    float voltage = gauge.cellVoltage();
    float percent = gauge.cellPercent();
    float rate = gauge.chargeRate();

    Serial.printf("Battery: %.2f V, %.1f%%, rate: %.2f %%/hr\n",
                  voltage, percent, rate);

    if (gauge.isActiveAlert()) {
        Serial.println("⚠️ LOW BATTERY ALERT!");
    }

    delay(5000);
}
