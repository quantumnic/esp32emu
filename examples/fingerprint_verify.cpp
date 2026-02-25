#include <Arduino.h>
#include <Adafruit_Fingerprint.h>

Adafruit_Fingerprint finger(nullptr);

void setup() {
    Serial.begin(115200);
    finger.begin(57600);

    if (!finger.verifyPassword()) {
        Serial.println("Fingerprint sensor not found!");
        return;
    }

    finger.getTemplateCount();
    Serial.print("Templates stored: ");
    Serial.println(finger.templateCount);
    Serial.println("Waiting for finger...");
}

void loop() {
    uint8_t p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) {
        delay(100);
        return;
    }
    if (p != FINGERPRINT_OK) return;

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK) return;

    p = finger.fingerFastSearch();
    if (p == FINGERPRINT_OK) {
        Serial.print("✅ Match! ID #");
        Serial.print(finger.fingerID);
        Serial.print(" (confidence: ");
        Serial.print(finger.confidence);
        Serial.println(")");
        finger.LEDcontrol(1, 0, 1, 1); // green
    } else {
        Serial.println("❌ No match");
        finger.LEDcontrol(1, 0, 3, 1); // red
    }
    delay(2000);
}
