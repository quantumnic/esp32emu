// SD Card Data Logger Example
// Demonstrates: SD library, file write/read, CSV logging
// Board: ESP32 (default) or any board with SPI

#include "Arduino.h"
#include "SD.h"

#define CS_PIN 5
#define LOG_FILE "/datalog.csv"

void setup() {
    Serial.begin(115200);
    Serial.println("SD Card Data Logger");
    Serial.println("==================");

    if (!SD.begin(CS_PIN)) {
        Serial.println("SD card initialization failed!");
        return;
    }
    Serial.println("SD card initialized.");

    // Write CSV header
    File f = SD.open(LOG_FILE, FILE_WRITE);
    if (f) {
        f.println("timestamp,temperature,humidity,pressure");
        f.close();
        Serial.println("Created log file with header.");
    }
}

void loop() {
    static int sample = 0;

    // Simulate sensor readings
    float temp = 20.0 + (random(100) / 10.0);
    float humidity = 40.0 + (random(400) / 10.0);
    float pressure = 1013.0 + (random(100) / 10.0);

    // Append to log
    File f = SD.open(LOG_FILE, FILE_APPEND);
    if (f) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%lu,%.1f,%.1f,%.1f",
                 millis(), temp, humidity, pressure);
        f.println(buf);
        f.close();

        Serial.print("Sample ");
        Serial.print(sample);
        Serial.print(": T=");
        Serial.print(temp);
        Serial.print("°C H=");
        Serial.print(humidity);
        Serial.print("% P=");
        Serial.print(pressure);
        Serial.println("hPa");
    }

    sample++;
    if (sample >= 5) {
        // Read back and display
        Serial.println("\n--- Log Contents ---");
        File rf = SD.open(LOG_FILE, FILE_READ);
        if (rf) {
            std::string content = rf.readString();
            Serial.println(content.c_str());
            rf.close();
        }

        // Cleanup
        SD.remove(LOG_FILE);
        SD.end();
        Serial.println("Done! SD card data logging complete.");
        while(1) delay(1000);
    }

    delay(1000);
}
