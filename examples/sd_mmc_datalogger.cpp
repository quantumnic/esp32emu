// Example: SD_MMC data logger
// Logs sensor readings to an SD card via the SDMMC interface.
#include <Arduino.h>
#include "SD_MMC.h"

void setup() {
    Serial.begin(115200);

    if (!SD_MMC.begin("/sdcard", true /* 1-bit mode */)) {
        Serial.println("SD_MMC mount failed!");
        return;
    }
    Serial.printf("SD_MMC mounted. Total: %llu MB, Used: %llu MB\n",
                  SD_MMC.totalBytes() / (1024 * 1024),
                  SD_MMC.usedBytes() / (1024 * 1024));

    // Create log directory
    SD_MMC.mkdir("/logs");

    // Write header if file doesn't exist
    if (!SD_MMC.exists("/logs/data.csv")) {
        auto f = SD_MMC.open("/logs/data.csv", "w");
        f.println("timestamp,temperature,humidity");
        f.close();
    }

    // Append a reading
    auto f = SD_MMC.open("/logs/data.csv", "a");
    if (f) {
        f.printf("%lu,23.5,48.2\n", millis());
        f.close();
        Serial.println("Data logged.");
    }

    // Read back
    auto r = SD_MMC.open("/logs/data.csv", "r");
    if (r) {
        Serial.println("--- Log contents ---");
        while (r.available()) {
            Serial.write((char)r.read());
        }
        r.close();
    }
}

void loop() {
    delay(10000);
}

#ifdef ESP32EMU_TEST
int main() {
    setup();
    return 0;
}
#endif
