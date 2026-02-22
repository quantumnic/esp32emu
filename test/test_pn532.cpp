#include "PN532.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    PN532 nfc;
    assert(!nfc.isBegun());

    nfc.begin();
    assert(nfc.isBegun());

    // Firmware version
    uint32_t ver = nfc.getFirmwareVersion();
    assert(ver != 0);

    // SAM config
    assert(nfc.SAMConfig());
    assert(nfc.isSAMConfigured());

    // No card present
    uint8_t uid[7];
    uint8_t uidLen;
    assert(!nfc.readPassiveTargetID(PN532::MIFARE_ISO14443A, uid, &uidLen, 100));

    // Inject a card UID
    uint8_t testUid[] = {0x04, 0xA2, 0xB3, 0xC4};
    nfc.injectUID(testUid, 4);
    assert(nfc.readPassiveTargetID(PN532::MIFARE_ISO14443A, uid, &uidLen));
    assert(uidLen == 4);
    assert(uid[0] == 0x04);
    assert(uid[3] == 0xC4);

    // Queue empties
    assert(!nfc.readPassiveTargetID(PN532::MIFARE_ISO14443A, uid, &uidLen, 0));

    // NTAG read/write
    uint8_t pageData[] = {0x01, 0x02, 0x03, 0x04};
    assert(nfc.ntag2xx_WritePage(4, pageData));
    uint8_t readBuf[4];
    assert(nfc.ntag2xx_ReadPage(4, readBuf));
    assert(memcmp(readBuf, pageData, 4) == 0);

    // MIFARE auth and block read/write
    nfc.setAuthSuccess(true);
    uint8_t key[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    assert(nfc.mifareclassic_AuthenticateBlock(uid, 4, 4, 0, key));

    uint8_t blockData[16] = {0};
    blockData[0] = 0xAA; blockData[15] = 0xBB;
    assert(nfc.mifareclassic_WriteDataBlock(4, blockData));
    uint8_t readBlock[16];
    assert(nfc.mifareclassic_ReadDataBlock(4, readBlock));
    assert(readBlock[0] == 0xAA && readBlock[15] == 0xBB);

    printf("test_pn532: all assertions passed\n");
    return 0;
}
