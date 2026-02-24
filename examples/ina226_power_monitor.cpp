#include <Arduino.h>
#include <INA226.h>

INA226 ina(0x40);

void setup() {
    Serial.begin(115200);
    Serial.println("INA226 Power Monitor");

    ina.begin();
    ina.calibrate(0.1f, 3.2f); // 100mOhm shunt, 3.2A max
    ina.setAverage(INA226::AVG_16);

    // Set over-current alert at 2A
    ina.setAlertType(INA226::ALERT_OVER_CURRENT);
    ina.setAlertLimit(2.0f);

    Serial.println("Calibrated: 0.1 Ohm shunt, 3.2A max");
}

void loop() {
    float busV = ina.getBusVoltage();
    float current = ina.getCurrent();
    float power = ina.getPower();

    Serial.print("Bus: ");
    Serial.print(busV, 2);
    Serial.print("V  Current: ");
    Serial.print(current, 3);
    Serial.print("A  Power: ");
    Serial.print(power, 2);
    Serial.println("W");

    if (ina.alertTriggered()) {
        Serial.println("⚠️  OVER-CURRENT ALERT!");
    }

    delay(1000);
}
