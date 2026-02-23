// SDS011 PM2.5/PM10 air quality sensor example
// Reads particulate matter with sleep/wake power management

#include <Arduino.h>
#include <SDS011.h>

SDS011 sds;
HardwareSerial mySerial(2);

void setup() {
    Serial.begin(115200);
    sds.begin(mySerial);
    sds.setWorkingPeriod(3); // Measure every 3 minutes

    Serial.println("SDS011 Air Quality Monitor");
    Serial.println("===========================");

    // Inject test values
    sds.test_setPM25(18.3f);
    sds.test_setPM10(32.1f);
}

void loop() {
    sds.wakeup();
    delay(30000); // Warm up for 30 seconds

    float pm25, pm10;
    int err = sds.read(&pm25, &pm10);

    if (err == 0) {
        Serial.print("PM2.5: ");
        Serial.print(pm25);
        Serial.print(" µg/m³  |  PM10: ");
        Serial.print(pm10);
        Serial.println(" µg/m³");

        // WHO guidelines: PM2.5 < 15, PM10 < 45
        if (pm25 < 15 && pm10 < 45) {
            Serial.println("  ✅ Within WHO guidelines");
        } else {
            Serial.println("  ⚠️  Exceeds WHO guidelines");
        }
    } else {
        Serial.println("Sensor read error!");
    }

    sds.sleep();
    delay(150000); // Sleep for 2.5 minutes
}
