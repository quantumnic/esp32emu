// esp32emu example — AS5600 Magnetic Rotary Encoder
#include <Arduino.h>
#include <AS5600.h>

AS5600 encoder;

void setup() {
    Serial.begin(115200);
    Serial.println("=== AS5600 Rotary Encoder ===");

    Wire.begin();
    if (!encoder.begin()) {
        Serial.println("❌ AS5600 not found!");
        return;
    }

    if (!encoder.detectMagnet()) {
        Serial.println("⚠️  No magnet detected!");
        if (encoder.magnetTooWeak()) Serial.println("   Magnet too weak");
        if (encoder.magnetTooStrong()) Serial.println("   Magnet too strong");
        return;
    }

    encoder.setDirection(0); // clockwise
    Serial.printf("AGC: %d | Magnitude: %d\n", encoder.readAGC(), encoder.readMagnitude());
    Serial.println("✅ Encoder ready");
}

void loop() {
    float degrees = encoder.readAngleDegrees();
    uint16_t raw = encoder.rawAngle();
    int32_t cumulative = encoder.getCumulativePosition();

    Serial.printf("Angle: %.1f° (raw: %d) | Cumulative: %ld\n",
                  degrees, raw, (long)cumulative);

    uint8_t status = encoder.getMagnetStatus();
    if (!(status & 0x20)) Serial.println("⚠️  Magnet lost!");

    delay(100);
}
