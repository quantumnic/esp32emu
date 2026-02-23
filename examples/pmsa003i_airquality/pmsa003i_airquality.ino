// PMSA003I Air Quality Monitor — reads PM1.0, PM2.5, PM10.0 particulate data
#include <Adafruit_PMSA003I.h>

Adafruit_PM25AQI aqi;

void setup() {
    Serial.begin(115200);
    Serial.println("PMSA003I Air Quality Monitor");

    if (!aqi.begin_I2C()) {
        Serial.println("ERROR: PMSA003I not found!");
        while (1) delay(10);
    }
    Serial.println("Sensor ready.");
}

void loop() {
    PM25_AQI_Data data;
    if (aqi.read(&data)) {
        Serial.println("── Air Quality ──");
        Serial.print("PM 1.0: "); Serial.print(data.pm10_standard);
        Serial.print(" µg/m³  (env: "); Serial.print(data.pm10_env); Serial.println(")");
        Serial.print("PM 2.5: "); Serial.print(data.pm25_standard);
        Serial.print(" µg/m³  (env: "); Serial.print(data.pm25_env); Serial.println(")");
        Serial.print("PM 10 : "); Serial.print(data.pm100_standard);
        Serial.print(" µg/m³  (env: "); Serial.print(data.pm100_env); Serial.println(")");

        Serial.println("── Particle Counts (per 0.1L) ──");
        Serial.print(">0.3µm: "); Serial.println(data.particles_03um);
        Serial.print(">0.5µm: "); Serial.println(data.particles_05um);
        Serial.print(">1.0µm: "); Serial.println(data.particles_10um);
        Serial.print(">2.5µm: "); Serial.println(data.particles_25um);
        Serial.print(">5.0µm: "); Serial.println(data.particles_50um);
        Serial.print(">10µm:  "); Serial.println(data.particles_100um);
        Serial.println();
    }
    delay(2000);
}
