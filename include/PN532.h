#pragma once
// esp32emu: PN532 NFC/RFID reader mock
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>

class PN532 {
public:
    PN532() : _begun(false), _firmwareVer{1, 6, 7, 0} {}

    void begin() {
        _begun = true;
        printf("[esp32emu] PN532: begin\n");
    }

    uint32_t getFirmwareVersion() {
        if (!_begun) return 0;
        return ((uint32_t)_firmwareVer[0] << 24) | ((uint32_t)_firmwareVer[1] << 16) |
               ((uint32_t)_firmwareVer[2] << 8) | _firmwareVer[3];
    }

    bool SAMConfig() {
        _samConfigured = true;
        printf("[esp32emu] PN532: SAM configured\n");
        return true;
    }

    bool readPassiveTargetID(uint8_t cardType, uint8_t* uid, uint8_t* uidLength, uint16_t timeout = 1000) {
        (void)cardType; (void)timeout;
        if (_injectedUids.empty()) return false;
        auto& u = _injectedUids.front();
        *uidLength = u.size();
        memcpy(uid, u.data(), u.size());
        _injectedUids.erase(_injectedUids.begin());
        return true;
    }

    bool ntag2xx_ReadPage(uint8_t page, uint8_t* buffer) {
        if (page < 64 && _pageData[page][0] != 0xFF) {
            memcpy(buffer, _pageData[page], 4);
            return true;
        }
        return false;
    }

    bool ntag2xx_WritePage(uint8_t page, const uint8_t* data) {
        if (page < 64) {
            memcpy(_pageData[page], data, 4);
            return true;
        }
        return false;
    }

    bool mifareclassic_AuthenticateBlock(uint8_t* uid, uint8_t uidLen, uint32_t blockNumber, uint8_t keyNumber, uint8_t* keyData) {
        (void)uid; (void)uidLen; (void)blockNumber; (void)keyNumber; (void)keyData;
        return _authSuccess;
    }

    bool mifareclassic_ReadDataBlock(uint8_t blockNumber, uint8_t* data) {
        if (blockNumber < 64) {
            memcpy(data, _mifareBlocks[blockNumber], 16);
            return true;
        }
        return false;
    }

    bool mifareclassic_WriteDataBlock(uint8_t blockNumber, uint8_t* data) {
        if (blockNumber < 64) {
            memcpy(_mifareBlocks[blockNumber], data, 16);
            return true;
        }
        return false;
    }

    // Test helpers
    void injectUID(const uint8_t* uid, uint8_t len) {
        _injectedUids.push_back(std::vector<uint8_t>(uid, uid + len));
    }
    void setAuthSuccess(bool s) { _authSuccess = s; }
    bool isBegun() const { return _begun; }
    bool isSAMConfigured() const { return _samConfigured; }
    void setPageData(uint8_t page, const uint8_t data[4]) { if (page < 64) memcpy(_pageData[page], data, 4); }
    void setMifareBlock(uint8_t block, const uint8_t data[16]) { if (block < 64) memcpy(_mifareBlocks[block], data, 16); }

    static constexpr uint8_t MIFARE_ISO14443A = 0x00;

private:
    bool _begun;
    bool _samConfigured = false;
    bool _authSuccess = true;
    uint8_t _firmwareVer[4];
    std::vector<std::vector<uint8_t>> _injectedUids;
    uint8_t _pageData[64][4] = {};
    uint8_t _mifareBlocks[64][16] = {};
};
