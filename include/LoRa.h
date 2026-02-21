#pragma once
// esp32emu: LoRa (SX1276/78) mock — SPI-based LoRa radio emulation
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <functional>

class LoRaClass {
public:
    int begin(long frequency) {
        frequency_ = frequency;
        initialized_ = true;
        printf("[esp32emu] LoRa: initialized at %ld Hz\n", frequency);
        return 1;
    }

    void end() { initialized_ = false; }

    int beginPacket(int implicitHeader = 0) {
        (void)implicitHeader;
        txBuf_.clear();
        inTx_ = true;
        return 1;
    }

    int endPacket(bool async = false) {
        (void)async;
        inTx_ = false;
        lastTxPacket_ = txBuf_;
        txCount_++;
        printf("[esp32emu] LoRa: sent %zu bytes\n", txBuf_.size());
        return 1;
    }

    size_t write(uint8_t byte) {
        if (inTx_) txBuf_.push_back(byte);
        return 1;
    }

    size_t write(const uint8_t* buf, size_t size) {
        for (size_t i = 0; i < size; i++) write(buf[i]);
        return size;
    }

    size_t print(const char* s) {
        return write(reinterpret_cast<const uint8_t*>(s), strlen(s));
    }

    size_t println(const char* s) {
        size_t n = print(s);
        n += write('\r');
        n += write('\n');
        return n;
    }

    int parsePacket(int size = 0) {
        (void)size;
        if (rxQueue_.empty()) return 0;
        rxBuf_ = rxQueue_.front();
        rxQueue_.erase(rxQueue_.begin());
        rxPos_ = 0;
        lastRssi_ = rxRssi_;
        lastSnr_ = rxSnr_;
        return (int)rxBuf_.size();
    }

    int available() {
        return (int)(rxBuf_.size() - rxPos_);
    }

    int read() {
        if (rxPos_ >= rxBuf_.size()) return -1;
        return rxBuf_[rxPos_++];
    }

    int peek() {
        if (rxPos_ >= rxBuf_.size()) return -1;
        return rxBuf_[rxPos_];
    }

    int packetRssi() const { return lastRssi_; }
    float packetSnr() const { return lastSnr_; }
    long packetFrequencyError() const { return freqError_; }

    // Configuration
    void setTxPower(int level, int outputPin = 0) { (void)outputPin; txPower_ = level; }
    void setFrequency(long frequency) { frequency_ = frequency; }
    void setSpreadingFactor(int sf) { sf_ = sf; }
    void setSignalBandwidth(long sbw) { bandwidth_ = sbw; }
    void setCodingRate4(int denominator) { codingRate_ = denominator; }
    void setSyncWord(int sw) { syncWord_ = sw; }
    void enableCrc() { crc_ = true; }
    void disableCrc() { crc_ = false; }
    void setPins(int ss = -1, int reset = -1, int dio0 = -1) {
        ssPin_ = ss; resetPin_ = reset; dio0Pin_ = dio0;
    }
    void setSPIFrequency(uint32_t freq) { spiFreq_ = freq; }
    void enableInvertIQ() { invertIQ_ = true; }
    void disableInvertIQ() { invertIQ_ = false; }

    void onReceive(std::function<void(int)> cb) { onReceiveCb_ = cb; }
    void onTxDone(std::function<void()> cb) { onTxDoneCb_ = cb; }

    void receive(int size = 0) { (void)size; receiving_ = true; }
    void idle() { receiving_ = false; }
    void sleep() { receiving_ = false; sleeping_ = true; }

    // Test helpers
    void injectPacket(const uint8_t* data, size_t len, int rssi = -50, float snr = 9.0f) {
        rxQueue_.emplace_back(data, data + len);
        rxRssi_ = rssi;
        rxSnr_ = snr;
        if (onReceiveCb_) onReceiveCb_((int)len);
    }
    void injectPacket(const char* str, int rssi = -50, float snr = 9.0f) {
        injectPacket(reinterpret_cast<const uint8_t*>(str), strlen(str), rssi, snr);
    }
    const std::vector<uint8_t>& getLastTxPacket() const { return lastTxPacket_; }
    uint32_t getTxCount() const { return txCount_; }
    bool isInitialized() const { return initialized_; }
    long getFrequency() const { return frequency_; }
    int getSpreadingFactor() const { return sf_; }
    long getBandwidth() const { return bandwidth_; }
    int getTxPower() const { return txPower_; }
    bool getCrc() const { return crc_; }

private:
    bool initialized_ = false;
    bool inTx_ = false;
    bool receiving_ = false;
    bool sleeping_ = false;
    long frequency_ = 0;
    int txPower_ = 17;
    int sf_ = 7;
    long bandwidth_ = 125000;
    int codingRate_ = 5;
    int syncWord_ = 0x12;
    bool crc_ = false;
    bool invertIQ_ = false;
    int ssPin_ = -1, resetPin_ = -1, dio0Pin_ = -1;
    uint32_t spiFreq_ = 0;
    long freqError_ = 0;

    std::vector<uint8_t> txBuf_;
    std::vector<uint8_t> lastTxPacket_;
    uint32_t txCount_ = 0;

    std::vector<std::vector<uint8_t>> rxQueue_;
    std::vector<uint8_t> rxBuf_;
    size_t rxPos_ = 0;
    int lastRssi_ = 0;
    float lastSnr_ = 0.0f;
    int rxRssi_ = -50;
    float rxSnr_ = 9.0f;

    std::function<void(int)> onReceiveCb_;
    std::function<void()> onTxDoneCb_;
};

extern LoRaClass LoRa;

#ifndef ESP32EMU_LORA_NO_GLOBAL
inline LoRaClass LoRa;
#endif
