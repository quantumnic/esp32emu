// esp32emu example: Adafruit PN532 NFC tag reader
// Reads NFC tags and displays UID, writes NDEF URI
#include "Adafruit_PN532.h"

#define PN532_IRQ   2
#define PN532_RESET 3

Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void setup() {
    Serial.begin(115200);
    Serial.println("Adafruit PN532 NFC Reader");

    nfc.begin();
    uint32_t ver = nfc.getFirmwareVersion();
    if (!ver) {
        Serial.println("PN532 not found!");
        while (1) delay(10);
    }
    Serial.printf("PN532 firmware: %d.%d\n", (ver >> 16) & 0xFF, (ver >> 8) & 0xFF);
    nfc.SAMConfig();
    Serial.println("Waiting for NFC tag...");
}

void loop() {
    uint8_t uid[7];
    uint8_t uidLength;

    if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
        Serial.printf("Tag found! UID (%d bytes): ", uidLength);
        for (uint8_t i = 0; i < uidLength; i++) {
            Serial.printf("%02X ", uid[i]);
        }
        Serial.println();

        if (uidLength == 4) {
            // Mifare Classic - try to read block 4
            uint8_t key[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            if (nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, key)) {
                uint8_t data[16];
                if (nfc.mifareclassic_ReadDataBlock(4, data)) {
                    Serial.print("Block 4: ");
                    for (int i = 0; i < 16; i++) Serial.printf("%02X ", data[i]);
                    Serial.println();
                }
            }
        }
        delay(1000);
    }
    delay(100);
}
