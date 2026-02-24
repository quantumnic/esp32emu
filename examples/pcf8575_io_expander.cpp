// esp32emu example — PCF8575 16-bit I2C I/O Expander
#include <Arduino.h>
#include <PCF8575.h>

PCF8575 expander(0x20);

void setup() {
    Serial.begin(115200);
    Serial.println("=== PCF8575 16-bit I/O Expander Demo ===");

    if (!expander.begin()) {
        Serial.println("❌ PCF8575 not found!");
        return;
    }
    Serial.printf("✅ PCF8575 found at 0x%02X\n", expander.getAddress());

    // Set all outputs LOW
    expander.write16(0x0000);
    Serial.printf("Port: 0x%04X\n", expander.read16());

    // Turn on pins one by one
    for (uint8_t i = 0; i < 16; i++) {
        expander.write(i, HIGH);
        Serial.printf("Pin %2d ON → Port: 0x%04X\n", i, expander.read16());
        delay(100);
    }

    // Toggle each pin
    Serial.println("\nToggling all pins:");
    for (uint8_t i = 0; i < 16; i++) {
        expander.toggle(i);
        Serial.printf("Toggle pin %2d → Port: 0x%04X\n", i, expander.read16());
    }
}

void loop() { delay(1000); }
