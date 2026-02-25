#pragma once
// esp32emu — Adafruit Fingerprint sensor (R307/R503/R502) mock

#include <cstdint>
#include <cstdio>
#include <cstring>

#define FINGERPRINT_OK                 0x00
#define FINGERPRINT_PACKETRECIEVEERR   0x01
#define FINGERPRINT_NOFINGER           0x02
#define FINGERPRINT_IMAGEFAIL          0x03
#define FINGERPRINT_IMAGEMESS          0x06
#define FINGERPRINT_FEATUREFAIL        0x07
#define FINGERPRINT_NOMATCH            0x08
#define FINGERPRINT_NOTFOUND           0x09
#define FINGERPRINT_ENROLLMISMATCH     0x0A
#define FINGERPRINT_BADLOCATION        0x0B
#define FINGERPRINT_DBRANGEFAIL        0x0C
#define FINGERPRINT_UPLOADFEATUREFAIL  0x0D
#define FINGERPRINT_PACKETRESPONSEFAIL 0x0E
#define FINGERPRINT_UPLOADFAIL         0x0F
#define FINGERPRINT_DELETEFAIL         0x10
#define FINGERPRINT_DBCLEARFAIL        0x11
#define FINGERPRINT_PASSFAIL           0x13
#define FINGERPRINT_INVALIDIMAGE       0x15
#define FINGERPRINT_FLASHERR           0x18
#define FINGERPRINT_INVALIDREG         0x1A
#define FINGERPRINT_ADDRCODE           0x20

#define FINGERPRINT_COMMANDPACKET      0x01
#define FINGERPRINT_DATAPACKET         0x02
#define FINGERPRINT_ACKPACKET          0x07
#define FINGERPRINT_ENDDATAPACKET      0x08

class Adafruit_Fingerprint {
public:
    uint16_t fingerID = 0;
    uint16_t confidence = 0;
    uint16_t templateCount = 0;
    uint16_t capacity = 200;
    uint8_t  status_reg = 0;
    uint8_t  system_id = 0;
    uint16_t finger_library_size = 200;
    uint8_t  security_level = 3;
    uint32_t device_addr = 0xFFFFFFFF;
    uint16_t packet_len = 64;
    uint16_t baud_rate = 57600;

    Adafruit_Fingerprint(void* serial, uint32_t password = 0x00000000) {
        (void)serial;
        password_ = password;
    }

    bool begin(uint32_t baud = 57600) {
        (void)baud;
        initialized_ = true;
        fprintf(stderr, "[esp32emu] Fingerprint: initialized at %u baud\n", baud);
        return true;
    }

    bool verifyPassword() {
        return initialized_;
    }

    uint8_t getParameters() {
        if (!initialized_) return FINGERPRINT_PACKETRECIEVEERR;
        return FINGERPRINT_OK;
    }

    uint8_t getImage() {
        if (!initialized_) return FINGERPRINT_PACKETRECIEVEERR;
        if (!finger_present_) return FINGERPRINT_NOFINGER;
        fprintf(stderr, "[esp32emu] Fingerprint: image captured\n");
        return FINGERPRINT_OK;
    }

    uint8_t image2Tz(uint8_t slot = 1) {
        (void)slot;
        if (!initialized_) return FINGERPRINT_PACKETRECIEVEERR;
        return FINGERPRINT_OK;
    }

    uint8_t fingerSearch(uint8_t slot = 1) {
        (void)slot;
        if (!initialized_) return FINGERPRINT_PACKETRECIEVEERR;
        if (match_id_ >= 0) {
            fingerID = (uint16_t)match_id_;
            confidence = match_confidence_;
            return FINGERPRINT_OK;
        }
        return FINGERPRINT_NOTFOUND;
    }

    uint8_t fingerFastSearch() {
        return fingerSearch();
    }

    uint8_t createModel() {
        if (!initialized_) return FINGERPRINT_PACKETRECIEVEERR;
        return FINGERPRINT_OK;
    }

    uint8_t storeModel(uint16_t id) {
        if (!initialized_) return FINGERPRINT_PACKETRECIEVEERR;
        if (id >= capacity) return FINGERPRINT_BADLOCATION;
        stored_[id] = true;
        template_count_++;
        fprintf(stderr, "[esp32emu] Fingerprint: stored model at #%u\n", id);
        return FINGERPRINT_OK;
    }

    uint8_t loadModel(uint16_t id) {
        if (!initialized_) return FINGERPRINT_PACKETRECIEVEERR;
        if (id >= capacity || !stored_[id]) return FINGERPRINT_BADLOCATION;
        return FINGERPRINT_OK;
    }

    uint8_t deleteModel(uint16_t id) {
        if (!initialized_) return FINGERPRINT_PACKETRECIEVEERR;
        if (id >= capacity) return FINGERPRINT_BADLOCATION;
        if (stored_[id]) { stored_[id] = false; template_count_--; }
        fprintf(stderr, "[esp32emu] Fingerprint: deleted model #%u\n", id);
        return FINGERPRINT_OK;
    }

    uint8_t emptyDatabase() {
        if (!initialized_) return FINGERPRINT_PACKETRECIEVEERR;
        memset(stored_, 0, sizeof(stored_));
        template_count_ = 0;
        fprintf(stderr, "[esp32emu] Fingerprint: database cleared\n");
        return FINGERPRINT_OK;
    }

    uint8_t getTemplateCount() {
        if (!initialized_) return FINGERPRINT_PACKETRECIEVEERR;
        templateCount = template_count_;
        return FINGERPRINT_OK;
    }

    uint8_t setSecurityLevel(uint8_t level) {
        security_level = level;
        return FINGERPRINT_OK;
    }

    uint8_t LEDcontrol(uint8_t control, uint8_t speed = 0, uint8_t color = 0, uint8_t count = 0) {
        (void)control; (void)speed; (void)color; (void)count;
        return FINGERPRINT_OK;
    }

    // ── Test helpers ──
    void esp32emu_set_finger_present(bool present) { finger_present_ = present; }
    void esp32emu_set_match(int16_t id, uint16_t conf) { match_id_ = id; match_confidence_ = conf; }
    void esp32emu_clear_match() { match_id_ = -1; match_confidence_ = 0; }
    uint16_t esp32emu_get_template_count() const { return template_count_; }
    bool esp32emu_is_stored(uint16_t id) const { return (id < 200) ? stored_[id] : false; }

private:
    uint32_t password_ = 0;
    bool initialized_ = false;
    bool finger_present_ = false;
    int16_t match_id_ = -1;
    uint16_t match_confidence_ = 0;
    uint16_t template_count_ = 0;
    bool stored_[200] = {};
};
