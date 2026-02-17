// Fake DHT22 Sensor → Serial Output + File Logging
// esp32emu run examples/temperature_logger/temperature_logger.ino
#include "Arduino.h"
#include <cstdio>
#include <cmath>

#define DHT_PIN 4

float fakeTemperature() {
    // Simulate temperature between 18-28°C with sine wave
    float base = 22.0;
    float variation = 5.0 * sin(millis() / 10000.0);
    float noise = (random(100) - 50) / 100.0;
    return base + variation + noise;
}

float fakeHumidity() {
    float base = 55.0;
    float variation = 10.0 * cos(millis() / 15000.0);
    float noise = (random(100) - 50) / 50.0;
    return base + variation + noise;
}

FILE* logFile = nullptr;
unsigned long lastRead = 0;
int readCount = 0;

void setup() {
    Serial.begin(115200);
    Serial.println("🌡️  DHT22 Temperature Logger");
    Serial.println("Reading every 2 seconds...\n");
    
    logFile = fopen("/tmp/esp32emu_temperature.csv", "w");
    if (logFile) {
        fprintf(logFile, "timestamp_ms,temperature_c,humidity_pct\n");
        Serial.println("\033[32m[INFO] Logging to /tmp/esp32emu_temperature.csv\033[0m");
    }
}

void loop() {
    if (millis() - lastRead >= 2000) {
        lastRead = millis();
        readCount++;

        float temp = fakeTemperature();
        float hum = fakeHumidity();

        // Colored output based on temperature
        if (temp > 26.0) {
            Serial.printf("\033[31m");  // Red for hot
        } else if (temp < 20.0) {
            Serial.printf("\033[36m");  // Cyan for cold
        } else {
            Serial.printf("\033[32m");  // Green for comfortable
        }

        Serial.printf("#%03d | 🌡️  %.1f°C | 💧 %.1f%% | %lums\033[0m\n",
                      readCount, temp, hum, millis());

        if (logFile) {
            fprintf(logFile, "%lu,%.2f,%.2f\n", millis(), temp, hum);
            fflush(logFile);
        }

        // Heat index calculation
        if (readCount % 5 == 0) {
            float hi = temp + 0.5555 * (6.11 * exp(5417.7530 * (1/273.16 - 1/(273.15+temp))) * hum/100 - 10);
            Serial.printf("  📊 Heat Index: %.1f°C\n", hi);
        }
    }

    delay(100);
}
