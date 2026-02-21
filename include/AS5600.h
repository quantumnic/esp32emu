#pragma once
// esp32emu — AS5600 Magnetic Rotary Encoder mock

#include <Arduino.h>
#include <Wire.h>
#include <cstdint>

class AS5600 {
public:
    AS5600(TwoWire* wire = &Wire) : _wire(wire) {}

    bool begin(int dirPin = -1) {
        _dirPin = dirPin;
        _initialized = true;
        return true;
    }

    bool isConnected() { return _initialized && _magnetDetected; }

    // Raw angle: 0-4095 (12-bit)
    uint16_t rawAngle() { return _rawAngle; }
    // Scaled angle: 0-4095 mapped to configured range
    uint16_t readAngle() { return _rawAngle; }

    // Degrees: 0.0 - 360.0
    float readAngleDegrees() { return _rawAngle * 360.0f / 4096.0f; }

    // Magnet status
    bool detectMagnet() { return _magnetDetected; }
    bool magnetTooStrong() { return _magnetTooStrong; }
    bool magnetTooWeak() { return _magnetTooWeak; }
    uint8_t getMagnetStatus() {
        uint8_t status = 0;
        if (_magnetDetected) status |= 0x20;
        if (_magnetTooStrong) status |= 0x08;
        if (_magnetTooWeak) status |= 0x10;
        return status;
    }

    // AGC (automatic gain control): 0-255
    uint8_t readAGC() { return _agc; }

    // Magnitude (CORDIC)
    uint16_t readMagnitude() { return _magnitude; }

    // Configuration
    void setStartPosition(uint16_t pos) { _startPos = pos & 0x0FFF; }
    uint16_t getStartPosition() { return _startPos; }
    void setStopPosition(uint16_t pos) { _stopPos = pos & 0x0FFF; }
    uint16_t getStopPosition() { return _stopPos; }
    void setMaxAngle(uint16_t angle) { _maxAngle = angle & 0x0FFF; }
    uint16_t getMaxAngle() { return _maxAngle; }

    // Direction
    void setDirection(uint8_t dir) { _direction = dir; }
    uint8_t getDirection() { return _direction; }

    // Cumulative position (multi-turn)
    int32_t getCumulativePosition() { return _cumulativePos; }
    void resetCumulativePosition(int32_t pos = 0) { _cumulativePos = pos; }

    // --- Test helpers ---
    void test_setRawAngle(uint16_t angle) { _rawAngle = angle & 0x0FFF; }
    void test_setMagnetDetected(bool d) { _magnetDetected = d; }
    void test_setMagnetTooStrong(bool s) { _magnetTooStrong = s; }
    void test_setMagnetTooWeak(bool w) { _magnetTooWeak = w; }
    void test_setAGC(uint8_t agc) { _agc = agc; }
    void test_setMagnitude(uint16_t mag) { _magnitude = mag; }
    void test_setCumulativePosition(int32_t pos) { _cumulativePos = pos; }
    bool test_isInitialized() const { return _initialized; }

private:
    TwoWire* _wire;
    int _dirPin = -1;
    bool _initialized = false;
    bool _magnetDetected = true;
    bool _magnetTooStrong = false;
    bool _magnetTooWeak = false;
    uint16_t _rawAngle = 0;
    uint8_t _agc = 128;
    uint16_t _magnitude = 2048;
    uint16_t _startPos = 0;
    uint16_t _stopPos = 0;
    uint16_t _maxAngle = 0;
    uint8_t _direction = 0;
    int32_t _cumulativePos = 0;
};
