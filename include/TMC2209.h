#pragma once
// esp32emu — TMC2209 stepper driver mock
// Trinamic TMC2209 UART stepper motor driver with StealthChop/SpreadCycle.

#include "Arduino.h"
#include <cstdint>
#include <cstdio>

class TMC2209 {
public:
    // Microstep resolutions
    enum MicrostepResolution : uint8_t {
        MICROSTEPS_1   = 0,
        MICROSTEPS_2   = 1,
        MICROSTEPS_4   = 2,
        MICROSTEPS_8   = 3,
        MICROSTEPS_16  = 4,
        MICROSTEPS_32  = 5,
        MICROSTEPS_64  = 6,
        MICROSTEPS_128 = 7,
        MICROSTEPS_256 = 8
    };

    // Standstill modes
    enum StandstillMode : uint8_t {
        NORMAL = 0,
        FREEWHEELING = 1,
        STRONG_BRAKING = 2,
        BRAKING = 3
    };

    TMC2209() = default;

    void setup(HardwareSerial& serial, long baud = 115200, uint8_t address = 0) {
        (void)serial;
        _baud = baud;
        _address = address;
        _setup_done = true;
        fprintf(stderr, "[esp32emu] TMC2209: setup addr=%d baud=%ld\n", address, baud);
    }

    bool isSetupAndCommunicating() const { return _setup_done && _communicating; }
    bool isCommunicating() const { return _communicating; }

    // Motor control
    void enable() { _enabled = true; }
    void disable() { _enabled = false; }
    bool isEnabled() const { return _enabled; }

    // Current
    void setRunCurrent(uint8_t percent) { _run_current = percent; }
    void setHoldCurrent(uint8_t percent) { _hold_current = percent; }
    uint8_t getRunCurrent() const { return _run_current; }
    uint8_t getHoldCurrent() const { return _hold_current; }

    // Microsteps
    void setMicrostepsPerStep(uint16_t microsteps) { _microsteps = microsteps; }
    uint16_t getMicrostepsPerStep() const { return _microsteps; }

    // StealthChop
    void enableStealthChop() { _stealthchop = true; }
    void disableStealthChop() { _stealthchop = false; }
    bool isStealthChopEnabled() const { return _stealthchop; }

    // CoolStep
    void enableCoolStep(uint8_t lower = 1, uint8_t upper = 0) {
        _coolstep = true; _cs_lower = lower; _cs_upper = upper;
    }
    void disableCoolStep() { _coolstep = false; }
    bool isCoolStepEnabled() const { return _coolstep; }

    // Standstill
    void setStandstillMode(StandstillMode mode) { _standstill = mode; }
    StandstillMode getStandstillMode() const { return _standstill; }

    // StallGuard
    void setStallGuardThreshold(uint8_t threshold) { _sg_threshold = threshold; }
    uint8_t getStallGuardThreshold() const { return _sg_threshold; }
    uint16_t getStallGuardResult() const { return _sg_result; }

    // Movement (VACTUAL mode)
    void moveAtVelocity(int32_t velocity) { _velocity = velocity; }
    int32_t getVelocity() const { return _velocity; }

    // Status
    uint8_t getVersion() const { return 0x21; } // TMC2209 version
    uint32_t getInterfaceTransmissionCounter() const { return _tx_count; }
    bool isStandstill() const { return _velocity == 0; }

    // Diagnostics
    struct Status {
        bool over_temperature_warning = false;
        bool over_temperature_shutdown = false;
        bool short_to_ground_a = false;
        bool short_to_ground_b = false;
        bool open_load_a = false;
        bool open_load_b = false;
        bool stall_guard = false;
    };
    Status getStatus() const { return _status; }

    // Test helpers
    void _setCommunicating(bool v) { _communicating = v; }
    void _setStallGuardResult(uint16_t v) { _sg_result = v; }
    void _setStatus(const Status& s) { _status = s; }
    void _incrTxCount() { _tx_count++; }

private:
    bool _setup_done = false;
    bool _communicating = true;
    bool _enabled = false;
    long _baud = 115200;
    uint8_t _address = 0;
    uint8_t _run_current = 50;
    uint8_t _hold_current = 25;
    uint16_t _microsteps = 256;
    bool _stealthchop = true;
    bool _coolstep = false;
    uint8_t _cs_lower = 0;
    uint8_t _cs_upper = 0;
    StandstillMode _standstill = NORMAL;
    uint8_t _sg_threshold = 0;
    uint16_t _sg_result = 0;
    int32_t _velocity = 0;
    uint32_t _tx_count = 0;
    Status _status = {};
};
