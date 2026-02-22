// PN532 NFC card reader example
#include "Arduino.h"
#include "PN532.h"

PN532 nfc;

void setup() {
    Serial.begin(115200);
    nfc.begin();

    uint32_t ver = nfc.getFirmwareVersion();
    printf("PN532 firmware: %d.%d.%d\n", (ver >> 16) & 0xFF, (ver >> 8) & 0xFF, ver & 0xFF);

    nfc.SAMConfig();

    // Simulate a card tap
    uint8_t fakeUid[] = {0x04, 0xA2, 0xB3, 0xC4, 0xD5, 0xE6, 0xF7};
    nfc.injectUID(fakeUid, 7);

    Serial.println("Waiting for NFC card...");
}

void loop() {
    uint8_t uid[7];
    uint8_t uidLength;

    if (nfc.readPassiveTargetID(PN532::MIFARE_ISO14443A, uid, &uidLength)) {
        printf("Card detected! UID (%d bytes):", uidLength);
        for (uint8_t i = 0; i < uidLength; i++)
            printf(" %02X", uid[i]);
        printf("\n");
    }
    delay(500);
}
