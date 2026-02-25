#pragma once
// esp32emu — TMCStepper library mock (TMC2209, TMC2208, TMC2130)

#include <Arduino.h>
#include <cstdint>

class TMC2209Stepper {
public:
    TMC2209Stepper(HardwareSerial* serial, float rsense, uint8_t addr = 0)
        : _serial(serial), _rsense(rsense), _addr(addr) {}
    TMC2209Stepper(uint16_t rxPin, uint16_t txPin, float rsense, uint8_t addr = 0)
        : _serial(nullptr), _rsense(rsense), _addr(addr) { (void)rxPin; (void)txPin; }

    bool begin() { _initialized = true; return true; }
    uint8_t test_connection() { return _initialized ? 0 : 1; }

    // Current
    void rms_current(uint16_t mA) { _rmsCurrent = mA; }
    uint16_t rms_current() const { return _rmsCurrent; }

    // Microsteps
    void microsteps(uint16_t ms) { _microsteps = ms; }
    uint16_t microsteps() const { return _microsteps; }

    // StealthChop / SpreadCycle
    void en_spreadCycle(bool en) { _spreadCycle = en; }
    bool en_spreadCycle() const { return _spreadCycle; }

    // StallGuard
    void SGTHRS(uint8_t val) { _sgthrs = val; }
    uint8_t SGTHRS() const { return _sgthrs; }
    uint16_t SG_RESULT() const { return _sgResult; }

    // VACTUAL for velocity mode
    void VACTUAL(int32_t v) { _vactual = v; }
    int32_t VACTUAL() const { return _vactual; }

    // CoolStep
    void TCOOLTHRS(uint32_t val) { _tcoolthrs = val; }
    uint32_t TCOOLTHRS() const { return _tcoolthrs; }

    // Status
    bool shaft() const { return _shaft; }
    void shaft(bool s) { _shaft = s; }
    uint32_t TSTEP() const { return _tstep; }

    // --- Test helpers ---
    void test_setSGResult(uint16_t v) { _sgResult = v; }
    void test_setTSTEP(uint32_t v) { _tstep = v; }
    bool test_isInitialized() const { return _initialized; }

private:
    HardwareSerial* _serial;
    float _rsense;
    uint8_t _addr;
    bool _initialized = false;
    uint16_t _rmsCurrent = 0;
    uint16_t _microsteps = 256;
    bool _spreadCycle = false;
    uint8_t _sgthrs = 0;
    uint16_t _sgResult = 0;
    int32_t _vactual = 0;
    uint32_t _tcoolthrs = 0;
    bool _shaft = false;
    uint32_t _tstep = 0;
};

// Alias for common usage
using TMC2208Stepper = TMC2209Stepper;
