#pragma once
#include <cstdint>

// TCA9548A I2C multiplexer mock — 8 channels, address 0x70-0x77
// Allows switching between up to 8 I2C buses from a single master bus

class TCA9548A {
public:
    explicit TCA9548A(uint8_t addr = 0x70) : addr_(addr) {}

    bool begin() { initialized_ = true; channel_mask_ = 0; return true; }

    // Select a single channel (0-7)
    void selectChannel(uint8_t ch) {
        if (ch < 8) channel_mask_ = (1 << ch);
    }

    // Enable/disable individual channels (bitmask)
    void setChannelMask(uint8_t mask) { channel_mask_ = mask; }
    uint8_t getChannelMask() const { return channel_mask_; }

    // Check if a channel is active
    bool isChannelActive(uint8_t ch) const {
        return ch < 8 && (channel_mask_ & (1 << ch));
    }

    // Close all channels
    void closeAll() { channel_mask_ = 0; }

    // Get the currently selected single channel (-1 if none or multiple)
    int8_t getChannel() const {
        if (channel_mask_ == 0) return -1;
        // Check if only one bit set
        if ((channel_mask_ & (channel_mask_ - 1)) != 0) return -1;
        for (uint8_t i = 0; i < 8; i++) {
            if (channel_mask_ & (1 << i)) return (int8_t)i;
        }
        return -1;
    }

    uint8_t getAddress() const { return addr_; }
    bool isInitialized() const { return initialized_; }

private:
    uint8_t addr_ = 0x70;
    bool initialized_ = false;
    uint8_t channel_mask_ = 0;
};
