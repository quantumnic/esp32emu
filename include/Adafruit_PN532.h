#pragma once
// esp32emu: Adafruit PN532 NFC/RFID reader mock (wraps PN532)
#include "PN532.h"
#include "Wire.h"

// Adafruit's PN532 library uses a different API surface than the raw PN532 library.
// This mock provides the Adafruit-specific interface.

#define PN532_MIFARE_ISO14443A  0x00
#define PN532_MIFARE_ISO14443B  0x03
#define PN532_GPIO_VALIDATIONBIT 0x80
#define PN532_GPIO_P30           0x01
#define PN532_GPIO_P31           0x02
#define PN532_GPIO_P32           0x04
#define PN532_GPIO_P33           0x08
#define PN532_GPIO_P34           0x10
#define PN532_GPIO_P35           0x20

class Adafruit_PN532 {
public:
    // I2C constructor
    Adafruit_PN532(uint8_t irq, uint8_t reset)
        : _irq(irq), _reset(reset), _spi_cs(-1), _begun(false) {}

    // SPI constructor
    Adafruit_PN532(uint8_t clk, uint8_t miso, uint8_t mosi, uint8_t ss)
        : _irq(-1), _reset(-1), _spi_cs(ss), _begun(false) {
        (void)clk; (void)miso; (void)mosi;
    }

    // HW SPI constructor
    Adafruit_PN532(uint8_t ss)
        : _irq(-1), _reset(-1), _spi_cs(ss), _begun(false) {}

    void begin() {
        _begun = true;
        printf("[esp32emu] Adafruit_PN532: initialized\n");
    }

    uint32_t getFirmwareVersion() {
        if (!_begun) return 0;
        return (1u << 24) | (6u << 16) | (7u << 8) | 0;
    }

    bool SAMConfig() {
        _samConfigured = true;
        printf("[esp32emu] Adafruit_PN532: SAM configured\n");
        return true;
    }

    bool readPassiveTargetID(uint8_t cardbaudrate, uint8_t* uid, uint8_t* uidLength,
                              uint16_t timeout = 0) {
        (void)cardbaudrate; (void)timeout;
        if (_uidQueue.empty()) {
            *uidLength = 0;
            return false;
        }
        auto& card = _uidQueue.front();
        *uidLength = card.size();
        memcpy(uid, card.data(), card.size());
        _uidQueue.erase(_uidQueue.begin());
        _lastUID = std::vector<uint8_t>(uid, uid + *uidLength);
        return true;
    }

    // NTAG2xx / Mifare Ultralight
    bool ntag2xx_ReadPage(uint8_t page, uint8_t* buffer) {
        auto it = _pages.find(page);
        if (it != _pages.end()) {
            memcpy(buffer, it->second, 4);
            return true;
        }
        memset(buffer, 0, 4);
        return true;
    }

    bool ntag2xx_WritePage(uint8_t page, const uint8_t* data) {
        memcpy(_pages[page], data, 4);
        return true;
    }

    // Mifare Classic
    bool mifareclassic_AuthenticateBlock(const uint8_t* uid, uint8_t uidLen,
                                          uint32_t blockNumber, uint8_t keyNumber,
                                          const uint8_t* keyData) {
        (void)uid; (void)uidLen; (void)blockNumber; (void)keyNumber; (void)keyData;
        _authenticated = true;
        return true;
    }

    bool mifareclassic_ReadDataBlock(uint8_t blockNumber, uint8_t* data) {
        auto it = _blocks.find(blockNumber);
        if (it != _blocks.end()) {
            memcpy(data, it->second.data(), 16);
        } else {
            memset(data, 0, 16);
        }
        return _authenticated;
    }

    bool mifareclassic_WriteDataBlock(uint8_t blockNumber, const uint8_t* data) {
        if (!_authenticated) return false;
        _blocks[blockNumber] = std::vector<uint8_t>(data, data + 16);
        return true;
    }

    bool mifareclassic_FormatNDEF() {
        _authenticated = true;
        return true;
    }

    bool mifareclassic_WriteNDEFURI(uint8_t sectorNumber, uint8_t uriIdentifier,
                                     const char* url) {
        (void)sectorNumber; (void)uriIdentifier;
        _ndefURI = url ? url : "";
        return true;
    }

    // GPIO
    bool writeGPIO(uint8_t pinstate) { _gpio = pinstate; return true; }
    uint8_t readGPIO() { return _gpio; }

    // Power down
    bool powerDownMode() { _powerDown = true; return true; }
    bool wakeup() { _powerDown = false; return _begun; }

    // Passive target presence
    bool inListPassiveTarget() { return !_uidQueue.empty(); }

    // ── Test helpers ──
    void emu_injectUID(const uint8_t* uid, uint8_t len) {
        _uidQueue.push_back(std::vector<uint8_t>(uid, uid + len));
    }
    bool emu_begun() const { return _begun; }
    bool emu_samConfigured() const { return _samConfigured; }
    bool emu_powerDown() const { return _powerDown; }
    const std::string& emu_ndefURI() const { return _ndefURI; }
    const std::vector<uint8_t>& emu_lastUID() const { return _lastUID; }

private:
    [[maybe_unused]] int8_t _irq, _reset, _spi_cs;
    bool _begun = false;
    bool _samConfigured = false;
    bool _authenticated = false;
    bool _powerDown = false;
    uint8_t _gpio = 0;
    std::string _ndefURI;
    std::vector<uint8_t> _lastUID;
    std::vector<std::vector<uint8_t>> _uidQueue;
    std::map<uint8_t, uint8_t[4]> _pages;
    std::map<uint8_t, std::vector<uint8_t>> _blocks;
};
