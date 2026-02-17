// I2C Bus Scanner — finds configured mock devices
// esp32emu run examples/i2c_scanner/i2c_scanner.ino
#include "Arduino.h"
#include "Wire.h"

void setup() {
    Serial.begin(115200);
    Wire.begin();

    // Register some mock I2C devices
    Wire.addDevice(0x27, [](const std::vector<uint8_t>&, uint8_t n) -> std::vector<uint8_t> {
        return std::vector<uint8_t>(n, 0xFF); // LCD backpack
    });
    Wire.addDevice(0x3C, [](const std::vector<uint8_t>&, uint8_t n) -> std::vector<uint8_t> {
        return std::vector<uint8_t>(n, 0x00); // OLED SSD1306
    });
    Wire.addDevice(0x68, [](const std::vector<uint8_t>&, uint8_t n) -> std::vector<uint8_t> {
        return std::vector<uint8_t>(n, 0x68); // MPU6050
    });
    Wire.addDevice(0x76, [](const std::vector<uint8_t>&, uint8_t n) -> std::vector<uint8_t> {
        return std::vector<uint8_t>(n, 0x58); // BMP280
    });

    Serial.println("🔍 I2C Scanner\n");
    Serial.println("Scanning...\n");

    int found = 0;
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        uint8_t error = Wire.endTransmission();

        if (error == 0) {
            Serial.printf("  \033[32m✓ Device found at 0x%02X", addr);
            // Identify known devices
            switch (addr) {
                case 0x27: Serial.print(" (LCD I2C Backpack)"); break;
                case 0x3C: Serial.print(" (SSD1306 OLED)"); break;
                case 0x68: Serial.print(" (MPU6050 IMU)"); break;
                case 0x76: Serial.print(" (BMP280 Pressure)"); break;
            }
            Serial.println("\033[0m");
            found++;
        }
    }

    Serial.printf("\n📊 Scan complete: %d device(s) found.\n", found);
}

void loop() {
    delay(5000);
    // Could re-scan periodically
}
