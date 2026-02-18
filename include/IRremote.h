#pragma once
#include <cstdint>
#include <cstdio>
#include <vector>
#include <queue>
#include <string>

/// @brief IRremote library mock for esp32emu.
/// Emulates infrared send/receive with test helpers.

// Protocol types
enum decode_type_t {
    UNKNOWN = -1,
    NEC = 1,
    SONY = 2,
    RC5 = 3,
    RC6 = 4,
    SAMSUNG = 5,
    LG = 6,
    JVC = 7,
    PANASONIC = 8
};

struct decode_results {
    decode_type_t decode_type = UNKNOWN;
    uint32_t value = 0;
    int bits = 0;
};

/// Receiver mock
class IRrecv {
public:
    IRrecv(int pin) : pin_(pin) {}

    void enableIRIn() { enabled_ = true; }

    int decode(decode_results* results) {
        if (!enabled_ || queue_.empty()) return 0;
        *results = queue_.front();
        queue_.pop();
        return 1;
    }

    void resume() { /* no-op in mock */ }

    // ── Test helpers ──
    void injectCode(decode_type_t type, uint32_t value, int bits = 32) {
        queue_.push({type, value, bits});
    }

    int getPin() const { return pin_; }
    bool isEnabled() const { return enabled_; }
    size_t pendingCount() const { return queue_.size(); }

private:
    int pin_;
    bool enabled_ = false;
    std::queue<decode_results> queue_;
};

/// Sender mock
class IRsend {
public:
    IRsend(int pin = 3) : pin_(pin) {}

    void begin() { active_ = true; }

    void sendNEC(uint32_t data, int nbits = 32) {
        log_.push_back({NEC, data, nbits});
        fprintf(stderr, "📡 IR TX: NEC 0x%08X (%d bits)\n", data, nbits);
    }

    void sendSony(uint32_t data, int nbits = 12) {
        log_.push_back({SONY, data, nbits});
        fprintf(stderr, "📡 IR TX: SONY 0x%08X (%d bits)\n", data, nbits);
    }

    void sendSamsung(uint32_t data, int nbits = 32) {
        log_.push_back({SAMSUNG, data, nbits});
        fprintf(stderr, "📡 IR TX: SAMSUNG 0x%08X (%d bits)\n", data, nbits);
    }

    void sendRC5(uint32_t data, int nbits = 12) {
        log_.push_back({RC5, data, nbits});
        fprintf(stderr, "📡 IR TX: RC5 0x%08X (%d bits)\n", data, nbits);
    }

    void sendRaw(const uint16_t buf[], unsigned int len, unsigned int freq) {
        (void)buf; (void)freq;
        rawCount_++;
        fprintf(stderr, "📡 IR TX: RAW %u pulses @ %uHz\n", len, freq);
    }

    // ── Test helpers ──
    const std::vector<decode_results>& getSentLog() const { return log_; }
    size_t getSentCount() const { return log_.size(); }
    size_t getRawCount() const { return rawCount_; }
    int getPin() const { return pin_; }
    bool isActive() const { return active_; }

    void clearLog() { log_.clear(); rawCount_ = 0; }

private:
    int pin_;
    bool active_ = false;
    std::vector<decode_results> log_;
    size_t rawCount_ = 0;
};
