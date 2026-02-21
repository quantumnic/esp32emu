#pragma once
// esp32emu — RCSwitch 433MHz RF mock

#include <Arduino.h>
#include <cstdint>
#include <vector>

class RCSwitch {
public:
    RCSwitch() = default;

    void enableReceive(int pin) { _rxPin = pin; _rxEnabled = true; }
    void disableReceive() { _rxEnabled = false; }

    void enableTransmit(int pin) { _txPin = pin; _txEnabled = true; }
    void disableTransmit() { _txEnabled = false; }

    bool available() { return _rxEnabled && _hasData; }

    unsigned long getReceivedValue() { return _receivedValue; }
    unsigned int getReceivedBitlength() { return _receivedBitLength; }
    unsigned int getReceivedDelay() { return _receivedDelay; }
    unsigned int getReceivedProtocol() { return _receivedProtocol; }
    unsigned int* getReceivedRawdata() { return _rawData.data(); }

    void resetAvailable() { _hasData = false; }

    // Transmit
    void send(unsigned long code, unsigned int length) {
        _lastSentCode = code;
        _lastSentLength = length;
        _sendCount++;
    }

    void send(const char* codeWord) {
        (void)codeWord;
        _sendCount++;
    }

    void sendTriState(const char* codeWord) {
        (void)codeWord;
        _sendCount++;
    }

    // Protocol / timing
    void setProtocol(int protocol) { _protocol = protocol; }
    void setProtocol(int protocol, int pulseLength) { _protocol = protocol; _pulseLength = pulseLength; }
    void setPulseLength(int length) { _pulseLength = length; }
    void setRepeatTransmit(int repeat) { _repeatTransmit = repeat; }

    // Switch helpers (Type A/B/C)
    void switchOn(const char* group, const char* device) { (void)group; (void)device; _sendCount++; }
    void switchOff(const char* group, const char* device) { (void)group; (void)device; _sendCount++; }
    void switchOn(int group, int device) { (void)group; (void)device; _sendCount++; }
    void switchOff(int group, int device) { (void)group; (void)device; _sendCount++; }

    // --- Test helpers ---
    void test_injectReceived(unsigned long value, unsigned int bitLen = 24,
                             unsigned int protocol = 1, unsigned int delay = 350) {
        _receivedValue = value;
        _receivedBitLength = bitLen;
        _receivedProtocol = protocol;
        _receivedDelay = delay;
        _hasData = true;
    }
    unsigned long test_getLastSentCode() const { return _lastSentCode; }
    unsigned int test_getLastSentLength() const { return _lastSentLength; }
    int test_getSendCount() const { return _sendCount; }
    bool test_isRxEnabled() const { return _rxEnabled; }
    bool test_isTxEnabled() const { return _txEnabled; }

private:
    bool _rxEnabled = false;
    bool _txEnabled = false;
    int _rxPin = -1;
    int _txPin = -1;
    bool _hasData = false;
    unsigned long _receivedValue = 0;
    unsigned int _receivedBitLength = 0;
    unsigned int _receivedDelay = 0;
    unsigned int _receivedProtocol = 0;
    std::vector<unsigned int> _rawData{64, 0};
    unsigned long _lastSentCode = 0;
    unsigned int _lastSentLength = 0;
    int _sendCount = 0;
    int _protocol = 1;
    int _pulseLength = 350;
    int _repeatTransmit = 10;
};
