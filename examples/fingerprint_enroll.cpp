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
    Serial.println("Fingerprint sensor ready");

    finger.getParameters();
    Serial.print("Capacity: ");
    Serial.println(finger.capacity);
    Serial.print("Security level: ");
    Serial.println(finger.security_level);

    finger.getTemplateCount();
    Serial.print("Templates stored: ");
    Serial.println(finger.templateCount);

    finger.LEDcontrol(1, 0, 2, 1); // blue breathing
    Serial.println("Place finger to enroll as ID #1...");
}

void loop() {
    uint8_t p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) {
        delay(100);
        return;
    }
    if (p != FINGERPRINT_OK) {
        Serial.println("Image error");
        return;
    }

    Serial.println("Image captured");
    p = finger.image2Tz(1);
    if (p != FINGERPRINT_OK) {
        Serial.println("Feature extraction failed");
        return;
    }

    Serial.println("Remove finger, then place again...");
    delay(2000);

    p = finger.getImage();
    if (p != FINGERPRINT_OK) return;

    p = finger.image2Tz(2);
    if (p != FINGERPRINT_OK) return;

    p = finger.createModel();
    if (p != FINGERPRINT_OK) {
        Serial.println("Model creation failed");
        return;
    }

    p = finger.storeModel(1);
    if (p == FINGERPRINT_OK) {
        Serial.println("✅ Fingerprint enrolled as ID #1!");
        finger.LEDcontrol(1, 0, 1, 1); // green
    } else {
        Serial.println("❌ Storage failed");
    }

    while (true) delay(1000); // done
}
