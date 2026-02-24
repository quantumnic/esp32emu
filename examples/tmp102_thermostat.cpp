#include <Arduino.h>
#include <TMP102.h>

TMP102 sensor(0x48);

void setup() {
    Serial.begin(115200);
    Serial.println("TMP102 Thermostat Example");

    sensor.begin();
    sensor.setConversionRate(2); // 4Hz
    sensor.setHighTempC(30.0f);
    sensor.setLowTempC(18.0f);

    Serial.print("High limit: ");
    Serial.print(sensor.getHighTempC());
    Serial.println("°C");
    Serial.print("Low limit: ");
    Serial.print(sensor.getLowTempC());
    Serial.println("°C");
}

void loop() {
    float tempC = sensor.readTempC();
    float tempF = sensor.readTempF();

    Serial.print("Temperature: ");
    Serial.print(tempC, 1);
    Serial.print("°C / ");
    Serial.print(tempF, 1);
    Serial.print("°F");

    if (sensor.alert()) {
        Serial.print("  ⚠️ ALERT!");
    }
    Serial.println();

    delay(1000);
}
