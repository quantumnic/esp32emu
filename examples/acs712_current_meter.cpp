#include <Arduino.h>
#include <ACS712.h>

ACS712 sensor(ACS712::ACS712_20A, 34);

void setup() {
    Serial.begin(115200);
    Serial.println("ACS712 Current Meter");

    sensor.begin();
    sensor.calibrate();
    Serial.print("Midpoint: ");
    Serial.print(sensor.getMidPoint());
    Serial.println("V");
}

void loop() {
    float dc = sensor.getCurrentDC();
    float ac = sensor.getCurrentAC();
    float voltage = sensor.getVoltage();

    Serial.print("DC: ");
    Serial.print(dc, 2);
    Serial.print("A  AC: ");
    Serial.print(ac, 2);
    Serial.print("A  Voltage: ");
    Serial.print(voltage, 3);
    Serial.println("V");

    delay(1000);
}
