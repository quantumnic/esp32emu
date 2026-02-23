#pragma once
// esp32emu mock: RadioLib — multi-protocol radio library (LoRa, FSK, etc.)
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <functional>

// Status codes
#define RADIOLIB_ERR_NONE                0
#define RADIOLIB_ERR_UNKNOWN            -1
#define RADIOLIB_ERR_PACKET_TOO_LONG    -2
#define RADIOLIB_ERR_TX_TIMEOUT         -3
#define RADIOLIB_ERR_RX_TIMEOUT         -4
#define RADIOLIB_ERR_CRC_MISMATCH       -5
#define RADIOLIB_ERR_INVALID_FREQUENCY  -6
#define RADIOLIB_ERR_INVALID_BANDWIDTH  -7
#define RADIOLIB_ERR_INVALID_SPREADING_FACTOR -8
#define RADIOLIB_ERR_INVALID_CODING_RATE -9
#define RADIOLIB_ERR_INVALID_OUTPUT_POWER -10
#define RADIOLIB_ERR_SPI_CMD_TIMEOUT    -11
#define RADIOLIB_ERR_SPI_CMD_INVALID    -12
#define RADIOLIB_ERR_CHIP_NOT_FOUND     -13

class Module {
public:
    Module(int cs, int irq, int rst, int gpio = -1)
        : _cs(cs), _irq(irq), _rst(rst), _gpio(gpio) {}
    int _cs, _irq, _rst, _gpio;
};

class PhysicalLayer {
public:
    virtual ~PhysicalLayer() = default;
    virtual int16_t transmit(const uint8_t* data, size_t len) = 0;
    virtual int16_t receive(uint8_t* data, size_t& len) = 0;
    virtual int16_t standby() = 0;
    virtual int16_t sleep() = 0;
};

class SX1276 : public PhysicalLayer {
public:
    SX1276(Module* mod) : _mod(mod) {}

    int16_t begin(float freq = 434.0, float bw = 125.0, uint8_t sf = 9,
                  uint8_t cr = 7, uint8_t syncWord = 0x12,
                  int8_t power = 10, uint16_t preamble = 8, uint8_t gain = 0) {
        _freq = freq; _bw = bw; _sf = sf; _cr = cr;
        _syncWord = syncWord; _power = power; _preamble = preamble;
        _initialized = true;
        return RADIOLIB_ERR_NONE;
    }

    int16_t transmit(const uint8_t* data, size_t len) override {
        if (!_initialized) return RADIOLIB_ERR_CHIP_NOT_FOUND;
        if (len > 255) return RADIOLIB_ERR_PACKET_TOO_LONG;
        _lastTx.assign(data, data + len);
        _txCount++;
        return RADIOLIB_ERR_NONE;
    }
    int16_t transmit(const char* str) {
        return transmit((const uint8_t*)str, strlen(str));
    }
    int16_t transmit(const std::string& str) {
        return transmit((const uint8_t*)str.c_str(), str.size());
    }

    int16_t receive(uint8_t* data, size_t& len) override {
        if (!_initialized) return RADIOLIB_ERR_CHIP_NOT_FOUND;
        if (_rxQueue.empty()) return RADIOLIB_ERR_RX_TIMEOUT;
        auto& pkt = _rxQueue.front();
        len = pkt.size();
        memcpy(data, pkt.data(), len);
        _rxQueue.erase(_rxQueue.begin());
        return RADIOLIB_ERR_NONE;
    }

    int16_t standby() override { _mode = 1; return RADIOLIB_ERR_NONE; }
    int16_t sleep() override { _mode = 2; return RADIOLIB_ERR_NONE; }

    int16_t setFrequency(float freq) {
        if (freq < 137.0 || freq > 1020.0) return RADIOLIB_ERR_INVALID_FREQUENCY;
        _freq = freq; return RADIOLIB_ERR_NONE;
    }
    int16_t setBandwidth(float bw) {
        _bw = bw; return RADIOLIB_ERR_NONE;
    }
    int16_t setSpreadingFactor(uint8_t sf) {
        if (sf < 6 || sf > 12) return RADIOLIB_ERR_INVALID_SPREADING_FACTOR;
        _sf = sf; return RADIOLIB_ERR_NONE;
    }
    int16_t setCodingRate(uint8_t cr) {
        if (cr < 5 || cr > 8) return RADIOLIB_ERR_INVALID_CODING_RATE;
        _cr = cr; return RADIOLIB_ERR_NONE;
    }
    int16_t setOutputPower(int8_t power) {
        if (power < -4 || power > 15) return RADIOLIB_ERR_INVALID_OUTPUT_POWER;
        _power = power; return RADIOLIB_ERR_NONE;
    }
    int16_t setSyncWord(uint8_t sw) { _syncWord = sw; return RADIOLIB_ERR_NONE; }
    int16_t setPreambleLength(uint16_t len) { _preamble = len; return RADIOLIB_ERR_NONE; }

    float getRSSI() const { return _testRSSI; }
    float getSNR() const { return _testSNR; }
    size_t getPacketLength() const { return _lastTx.size(); }

    // DIO interrupt
    void setDio0Action(void (*func)()) { _dio0Action = func; }
    void setDio1Action(void (*func)()) { _dio1Action = func; }

    int16_t startReceive() { _mode = 3; return RADIOLIB_ERR_NONE; }
    int16_t readData(uint8_t* data, size_t len) {
        if (_rxQueue.empty()) return RADIOLIB_ERR_RX_TIMEOUT;
        auto& pkt = _rxQueue.front();
        size_t copy = (len < pkt.size()) ? len : pkt.size();
        memcpy(data, pkt.data(), copy);
        _rxQueue.erase(_rxQueue.begin());
        return RADIOLIB_ERR_NONE;
    }

    // ── Test helpers ──
    void _test_injectRx(const uint8_t* data, size_t len) {
        _rxQueue.push_back(std::vector<uint8_t>(data, data + len));
    }
    void _test_injectRx(const std::string& s) {
        _test_injectRx((const uint8_t*)s.c_str(), s.size());
    }
    void _test_setRSSI(float rssi) { _testRSSI = rssi; }
    void _test_setSNR(float snr) { _testSNR = snr; }
    std::vector<uint8_t> _test_getLastTx() const { return _lastTx; }
    uint32_t _test_getTxCount() const { return _txCount; }
    float _test_getFrequency() const { return _freq; }
    float _test_getBandwidth() const { return _bw; }
    uint8_t _test_getSpreadingFactor() const { return _sf; }
    int8_t _test_getPower() const { return _power; }

private:
    Module* _mod;
    bool _initialized = false;
    int _mode = 0; // 0=idle, 1=standby, 2=sleep, 3=rx
    float _freq = 434.0, _bw = 125.0;
    uint8_t _sf = 9, _cr = 7, _syncWord = 0x12;
    int8_t _power = 10;
    uint16_t _preamble = 8;
    std::vector<uint8_t> _lastTx;
    uint32_t _txCount = 0;
    std::vector<std::vector<uint8_t>> _rxQueue;
    float _testRSSI = -60.0;
    float _testSNR = 10.0;
    void (*_dio0Action)() = nullptr;
    void (*_dio1Action)() = nullptr;
};

// SX1278 is pin-compatible with SX1276 for most uses
using SX1278 = SX1276;

class SX1262 : public PhysicalLayer {
public:
    SX1262(Module* mod) : _mod(mod) {}

    int16_t begin(float freq = 434.0, float bw = 125.0, uint8_t sf = 9,
                  uint8_t cr = 7, uint8_t syncWord = 0x34,
                  int8_t power = 10, uint16_t preamble = 8) {
        _freq = freq; _bw = bw; _sf = sf; _cr = cr;
        _syncWord = syncWord; _power = power;
        _initialized = true;
        return RADIOLIB_ERR_NONE;
    }

    int16_t transmit(const uint8_t* data, size_t len) override {
        if (!_initialized) return RADIOLIB_ERR_CHIP_NOT_FOUND;
        if (len > 255) return RADIOLIB_ERR_PACKET_TOO_LONG;
        _lastTx.assign(data, data + len);
        _txCount++;
        return RADIOLIB_ERR_NONE;
    }
    int16_t transmit(const char* str) {
        return transmit((const uint8_t*)str, strlen(str));
    }

    int16_t receive(uint8_t* data, size_t& len) override {
        if (_rxQueue.empty()) return RADIOLIB_ERR_RX_TIMEOUT;
        auto& pkt = _rxQueue.front();
        len = pkt.size();
        memcpy(data, pkt.data(), len);
        _rxQueue.erase(_rxQueue.begin());
        return RADIOLIB_ERR_NONE;
    }

    int16_t standby() override { return RADIOLIB_ERR_NONE; }
    int16_t sleep() override { return RADIOLIB_ERR_NONE; }

    int16_t setFrequency(float freq) { _freq = freq; return RADIOLIB_ERR_NONE; }
    int16_t setBandwidth(float bw) { _bw = bw; return RADIOLIB_ERR_NONE; }
    int16_t setSpreadingFactor(uint8_t sf) { _sf = sf; return RADIOLIB_ERR_NONE; }
    int16_t setOutputPower(int8_t power) { _power = power; return RADIOLIB_ERR_NONE; }

    float getRSSI() const { return _testRSSI; }
    float getSNR() const { return _testSNR; }

    void _test_injectRx(const std::string& s) {
        _rxQueue.push_back(std::vector<uint8_t>(s.begin(), s.end()));
    }
    void _test_setRSSI(float r) { _testRSSI = r; }
    void _test_setSNR(float s) { _testSNR = s; }
    std::vector<uint8_t> _test_getLastTx() const { return _lastTx; }
    uint32_t _test_getTxCount() const { return _txCount; }

private:
    Module* _mod;
    bool _initialized = false;
    float _freq = 434.0, _bw = 125.0;
    uint8_t _sf = 9, _cr = 7, _syncWord = 0x34;
    int8_t _power = 10;
    std::vector<uint8_t> _lastTx;
    uint32_t _txCount = 0;
    std::vector<std::vector<uint8_t>> _rxQueue;
    float _testRSSI = -60.0;
    float _testSNR = 10.0;
};
