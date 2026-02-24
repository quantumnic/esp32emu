#include <Arduino.h>
#include <Adafruit_MPRLS.h>

Adafruit_MPRLS mpr;

void setup() {
    Serial.begin(115200);
    Serial.println("MPRLS Ported Pressure Sensor");

    if (!mpr.begin()) {
        Serial.println("Failed to find MPRLS!");
        while (1) delay(10);
    }
    Serial.println("MPRLS initialized");
}

void loop() {
    float pressure_hpa = mpr.readPressure();
    float pressure_psi = pressure_hpa / 68.947572932;

    Serial.print("Pressure: "); Serial.print(pressure_hpa, 2); Serial.println(" hPa");
    Serial.print("Pressure: "); Serial.print(pressure_psi, 4); Serial.println(" PSI");

    uint8_t status = mpr.lastStatus();
    if (status != 0x40) {
        Serial.print("⚠️ Sensor status: 0x");
        Serial.println(status, HEX);
    }

    Serial.println("---");
    delay(1000);
}
