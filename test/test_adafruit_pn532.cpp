#include "Adafruit_PN532.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // I2C constructor
    Adafruit_PN532 nfc(2, 3);  // IRQ=2, RESET=3
    assert(!nfc.emu_begun());

    nfc.begin();
    assert(nfc.emu_begun());

    // Firmware version
    uint32_t ver = nfc.getFirmwareVersion();
    assert(ver != 0);
    assert(((ver >> 24) & 0xFF) == 1);  // IC
    assert(((ver >> 16) & 0xFF) == 6);  // firmware major

    // SAM config
    assert(nfc.SAMConfig());
    assert(nfc.emu_samConfigured());

    // No card present
    uint8_t uid[7];
    uint8_t uidLen;
    assert(!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, 100));

    // Inject a card UID
    uint8_t testUid[] = {0xDE, 0xAD, 0xBE, 0xEF};
    nfc.emu_injectUID(testUid, 4);
    assert(nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen));
    assert(uidLen == 4);
    assert(uid[0] == 0xDE && uid[3] == 0xEF);

    // Last UID saved
    assert(nfc.emu_lastUID().size() == 4);
    assert(nfc.emu_lastUID()[0] == 0xDE);

    // Queue empties after read
    assert(!nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLen, 0));

    // NTAG read/write
    uint8_t pageData[] = {0x01, 0x02, 0x03, 0x04};
    assert(nfc.ntag2xx_WritePage(4, pageData));
    uint8_t readBuf[4];
    assert(nfc.ntag2xx_ReadPage(4, readBuf));
    assert(readBuf[0] == 0x01 && readBuf[3] == 0x04);

    // Mifare Classic auth + read/write
    uint8_t key[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    assert(nfc.mifareclassic_AuthenticateBlock(testUid, 4, 4, 0, key));

    uint8_t blockData[16] = {0};
    blockData[0] = 0xCA; blockData[15] = 0xFE;
    assert(nfc.mifareclassic_WriteDataBlock(4, blockData));

    uint8_t readBlock[16];
    assert(nfc.mifareclassic_ReadDataBlock(4, readBlock));
    assert(readBlock[0] == 0xCA && readBlock[15] == 0xFE);

    // NDEF URI
    assert(nfc.mifareclassic_WriteNDEFURI(1, 0x04, "adafruit.com"));
    assert(nfc.emu_ndefURI() == "adafruit.com");

    // GPIO
    assert(nfc.writeGPIO(0x07));
    assert(nfc.readGPIO() == 0x07);

    // Power down / wakeup
    assert(nfc.powerDownMode());
    assert(nfc.emu_powerDown());
    assert(nfc.wakeup());
    assert(!nfc.emu_powerDown());

    // SPI constructor variant
    Adafruit_PN532 nfc_spi(10);
    nfc_spi.begin();
    assert(nfc_spi.emu_begun());

    printf("test_adafruit_pn532: all assertions passed\n");
    return 0;
}
