// esp32emu test — MFRC522 RFID Reader
#include <cassert>
#include <cstdio>
#include <cstring>
#define ESP32EMU_MOCK
#include "Arduino.h"
#include "MFRC522.h"

int main() {
    // Test init
    {
        MFRC522 rfid(5, 27);
        assert(!rfid.test_isInitialized());
        rfid.PCD_Init();
        assert(rfid.test_isInitialized());
        assert(rfid.PCD_PerformSelfTest());
    }

    // Test no card present
    {
        MFRC522 rfid(5, 27);
        rfid.PCD_Init();
        assert(!rfid.PICC_IsNewCardPresent());
    }

    // Test card detection and UID read
    {
        MFRC522 rfid(5, 27);
        rfid.PCD_Init();
        uint8_t uid[] = {0x01, 0x02, 0x03, 0x04};
        rfid.test_setCardPresent(true);
        rfid.test_setUid(uid, 4, 0x08);
        assert(rfid.PICC_IsNewCardPresent());
        assert(rfid.PICC_ReadCardSerial());
        assert(rfid.uid.size == 4);
        assert(rfid.uid.uidByte[0] == 0x01);
        assert(rfid.uid.uidByte[3] == 0x04);
        assert(rfid.uid.sak == 0x08);
    }

    // Test PICC type
    {
        MFRC522 rfid;
        assert(rfid.PICC_GetType(0x08) == MFRC522::PICC_TYPE_MIFARE_1K);
        assert(rfid.PICC_GetType(0x18) == MFRC522::PICC_TYPE_MIFARE_4K);
        assert(rfid.PICC_GetType(0x00) == MFRC522::PICC_TYPE_MIFARE_UL);
        const char* name = rfid.PICC_GetTypeName(MFRC522::PICC_TYPE_MIFARE_1K);
        assert(strcmp(name, "MIFARE 1KB") == 0);
    }

    // Test block read/write
    {
        MFRC522 rfid;
        rfid.PCD_Init();
        uint8_t data[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
        assert(rfid.MIFARE_Write(4, data, 16) == MFRC522::STATUS_OK);

        uint8_t buf[18];
        uint8_t bufSize = 18;
        assert(rfid.MIFARE_Read(4, buf, &bufSize) == MFRC522::STATUS_OK);
        assert(bufSize == 18);
        assert(buf[0] == 1 && buf[15] == 16);
    }

    // Test authentication
    {
        MFRC522 rfid;
        rfid.PCD_Init();
        MFRC522::MIFARE_Key key;
        memset(key.keyByte, 0xFF, 6);
        assert(rfid.PCD_Authenticate(0x60, 4, &key, &rfid.uid) == MFRC522::STATUS_OK);

        rfid.test_setAuthResult(MFRC522::STATUS_ERROR);
        assert(rfid.PCD_Authenticate(0x60, 4, &key, &rfid.uid) == MFRC522::STATUS_ERROR);
    }

    // Test halt
    {
        MFRC522 rfid;
        rfid.PCD_Init();
        rfid.test_setCardPresent(true);
        assert(rfid.PICC_IsNewCardPresent());
        rfid.PICC_HaltA();
        assert(!rfid.PICC_IsNewCardPresent());
    }

    printf("test_mfrc522: all assertions passed\n");
    return 0;
}
