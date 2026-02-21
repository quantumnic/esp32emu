// esp32emu example — MFRC522 RFID Access Control
#include <Arduino.h>
#include <MFRC522.h>

#define SS_PIN  5
#define RST_PIN 27

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

// Authorized UIDs
const uint8_t authorizedUID[][4] = {
    {0xDE, 0xAD, 0xBE, 0xEF},
    {0x01, 0x02, 0x03, 0x04}
};

bool isAuthorized(uint8_t* uid, uint8_t size) {
    for (auto& auth : authorizedUID) {
        if (size == 4 && memcmp(uid, auth, 4) == 0) return true;
    }
    return false;
}

void setup() {
    Serial.begin(115200);
    Serial.println("=== RFID Access Control ===");
    SPI.begin();
    rfid.PCD_Init();
    for (uint8_t i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
    Serial.println("✅ MFRC522 ready. Scan a card...");
}

void loop() {
    if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial())
        return;

    Serial.print("Card UID: ");
    for (uint8_t i = 0; i < rfid.uid.size; i++) {
        Serial.printf("%02X ", rfid.uid.uidByte[i]);
    }
    Serial.println();

    auto type = rfid.PICC_GetType(rfid.uid.sak);
    Serial.printf("Type: %s\n", rfid.PICC_GetTypeName(type));

    if (isAuthorized(rfid.uid.uidByte, rfid.uid.size)) {
        Serial.println("🟢 ACCESS GRANTED");
    } else {
        Serial.println("🔴 ACCESS DENIED");
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    delay(1000);
}
