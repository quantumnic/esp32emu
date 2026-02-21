#pragma once
// esp32emu — MFRC522 RFID Reader mock

#include <Arduino.h>
#include <SPI.h>
#include <cstdint>
#include <cstring>
#include <vector>

class MFRC522 {
public:
    // Status codes
    enum StatusCode : uint8_t {
        STATUS_OK = 0,
        STATUS_ERROR = 1,
        STATUS_COLLISION = 2,
        STATUS_TIMEOUT = 3,
        STATUS_NO_ROOM = 4,
        STATUS_INTERNAL_ERROR = 5,
        STATUS_INVALID = 6,
        STATUS_CRC_WRONG = 7,
        STATUS_MIFARE_NACK = 0xff
    };

    // PICC types
    enum PICC_Type : uint8_t {
        PICC_TYPE_UNKNOWN = 0,
        PICC_TYPE_ISO_14443_4 = 1,
        PICC_TYPE_ISO_18092 = 2,
        PICC_TYPE_MIFARE_MINI = 3,
        PICC_TYPE_MIFARE_1K = 4,
        PICC_TYPE_MIFARE_4K = 5,
        PICC_TYPE_MIFARE_UL = 6,
        PICC_TYPE_MIFARE_PLUS = 7,
        PICC_TYPE_MIFARE_DESFIRE = 8,
        PICC_TYPE_TNP3XXX = 9,
        PICC_TYPE_NOT_COMPLETE = 0xff
    };

    struct Uid {
        uint8_t size = 0;
        uint8_t uidByte[10] = {};
        uint8_t sak = 0;
    };

    struct MIFARE_Key {
        uint8_t keyByte[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    };

    Uid uid;

    MFRC522() : _ss(0), _rst(0) {}
    MFRC522(uint8_t ss, uint8_t rst) : _ss(ss), _rst(rst) {}

    void PCD_Init() { _initialized = true; }
    void PCD_Init(uint8_t /*ss*/, uint8_t /*rst*/) { _initialized = true; }

    bool PCD_PerformSelfTest() { return _initialized; }

    String PCD_GetVersionString() { return "MFRC522 v2.0 (emulated)"; }

    bool PICC_IsNewCardPresent() {
        if (!_initialized || !_cardPresent) return false;
        return true;
    }

    bool PICC_ReadCardSerial() {
        if (!_cardPresent) return false;
        uid.size = _mockUidSize;
        memcpy(uid.uidByte, _mockUid, _mockUidSize);
        uid.sak = _mockSak;
        return true;
    }

    PICC_Type PICC_GetType(uint8_t sak) {
        if (sak == 0x08) return PICC_TYPE_MIFARE_1K;
        if (sak == 0x18) return PICC_TYPE_MIFARE_4K;
        if (sak == 0x00) return PICC_TYPE_MIFARE_UL;
        return PICC_TYPE_UNKNOWN;
    }

    const char* PICC_GetTypeName(PICC_Type type) {
        switch (type) {
            case PICC_TYPE_MIFARE_MINI: return "MIFARE Mini";
            case PICC_TYPE_MIFARE_1K: return "MIFARE 1KB";
            case PICC_TYPE_MIFARE_4K: return "MIFARE 4KB";
            case PICC_TYPE_MIFARE_UL: return "MIFARE Ultralight";
            case PICC_TYPE_MIFARE_DESFIRE: return "MIFARE DESFire";
            default: return "Unknown";
        }
    }

    StatusCode PCD_Authenticate(uint8_t /*command*/, uint8_t /*blockAddr*/,
                                 MIFARE_Key* /*key*/, Uid* /*uid*/) {
        return _authResult;
    }

    StatusCode MIFARE_Read(uint8_t blockAddr, uint8_t* buffer, uint8_t* bufferSize) {
        if (blockAddr >= 64 || *bufferSize < 18) return STATUS_ERROR;
        memset(buffer, 0, 18);
        if (blockAddr < (int)_blockData.size() && !_blockData[blockAddr].empty()) {
            memcpy(buffer, _blockData[blockAddr].data(),
                   std::min((size_t)16, _blockData[blockAddr].size()));
        }
        *bufferSize = 18; // 16 data + 2 CRC
        return STATUS_OK;
    }

    StatusCode MIFARE_Write(uint8_t blockAddr, uint8_t* buffer, uint8_t bufferSize) {
        if (blockAddr >= 64 || bufferSize != 16) return STATUS_ERROR;
        if (blockAddr >= _blockData.size()) _blockData.resize(blockAddr + 1);
        _blockData[blockAddr].assign(buffer, buffer + 16);
        return STATUS_OK;
    }

    void PCD_StopCrypto1() {}

    void PICC_HaltA() { _cardPresent = false; }

    void PICC_DumpToSerial(Uid* /*uid*/) {
        Serial.println("[MFRC522] Card dump (emulated)");
    }

    // --- Test helpers ---
    void test_setCardPresent(bool present) { _cardPresent = present; }
    void test_setUid(const uint8_t* u, uint8_t size, uint8_t sak = 0x08) {
        _mockUidSize = size;
        memcpy(_mockUid, u, size);
        _mockSak = sak;
    }
    void test_setAuthResult(StatusCode r) { _authResult = r; }
    void test_setBlockData(uint8_t block, const uint8_t* data, size_t len) {
        if (block >= _blockData.size()) _blockData.resize(block + 1);
        _blockData[block].assign(data, data + len);
    }
    bool test_isInitialized() const { return _initialized; }

private:
    uint8_t _ss, _rst;
    bool _initialized = false;
    bool _cardPresent = false;
    uint8_t _mockUid[10] = {0xDE, 0xAD, 0xBE, 0xEF};
    uint8_t _mockUidSize = 4;
    uint8_t _mockSak = 0x08;
    StatusCode _authResult = STATUS_OK;
    std::vector<std::vector<uint8_t>> _blockData;
};
