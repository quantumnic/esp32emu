#pragma once
// esp32emu — PID_v1 controller mock
// Arduino PID Library by Brett Beauregard.

#include "Arduino.h"
#include <cstdio>

#define AUTOMATIC 1
#define MANUAL    0
#define DIRECT    0
#define REVERSE   1
#define P_ON_M    0
#define P_ON_E    1

class PID {
public:
    PID(double* input, double* output, double* setpoint,
        double kp, double ki, double kd, int pOn, int direction)
        : _input(input), _output(output), _setpoint(setpoint),
          _kp(kp), _ki(ki), _kd(kd), _pOn(pOn), _direction(direction) {
        _outMin = 0; _outMax = 255;
        _sampleTime = 100;
        _lastTime = millis() - _sampleTime;
        _lastInput = *_input;
        _outputSum = 0;
        _mode = MANUAL;
        if (_direction == REVERSE) { _kp = -_kp; _ki = -_ki; _kd = -_kd; }
        fprintf(stderr, "[esp32emu] PID: created Kp=%.2f Ki=%.2f Kd=%.2f\n", kp, ki, kd);
    }

    PID(double* input, double* output, double* setpoint,
        double kp, double ki, double kd, int direction)
        : PID(input, output, setpoint, kp, ki, kd, P_ON_E, direction) {}

    bool Compute() {
        if (_mode == MANUAL) return false;
        unsigned long now = millis();
        unsigned long elapsed = now - _lastTime;
        if (elapsed < (unsigned long)_sampleTime) return false;

        double input = *_input;
        double error = *_setpoint - input;
        double dInput = input - _lastInput;

        _outputSum += _ki * error;
        if (_pOn == P_ON_M) _outputSum -= _kp * dInput;
        if (_outputSum > _outMax) _outputSum = _outMax;
        else if (_outputSum < _outMin) _outputSum = _outMin;

        double output;
        if (_pOn == P_ON_E) output = _kp * error;
        else output = 0;
        output += _outputSum - _kd * dInput;

        if (output > _outMax) output = _outMax;
        else if (output < _outMin) output = _outMin;

        *_output = output;
        _lastInput = input;
        _lastTime = now;
        _computeCount++;
        return true;
    }

    void SetMode(int mode) {
        if (mode == AUTOMATIC && _mode == MANUAL) {
            // Re-init
            _outputSum = *_output;
            _lastInput = *_input;
            if (_outputSum > _outMax) _outputSum = _outMax;
            else if (_outputSum < _outMin) _outputSum = _outMin;
        }
        _mode = mode;
    }

    void SetOutputLimits(double min, double max) {
        if (min >= max) return;
        _outMin = min; _outMax = max;
        if (*_output > _outMax) *_output = _outMax;
        else if (*_output < _outMin) *_output = _outMin;
        if (_outputSum > _outMax) _outputSum = _outMax;
        else if (_outputSum < _outMin) _outputSum = _outMin;
    }

    void SetTunings(double kp, double ki, double kd, int pOn = P_ON_E) {
        if (kp < 0 || ki < 0 || kd < 0) return;
        _pOn = pOn;
        double sampleSec = (double)_sampleTime / 1000.0;
        _kp = kp; _ki = ki * sampleSec; _kd = kd / sampleSec;
        if (_direction == REVERSE) { _kp = -_kp; _ki = -_ki; _kd = -_kd; }
    }

    void SetSampleTime(int ms) {
        if (ms <= 0) return;
        double ratio = (double)ms / (double)_sampleTime;
        _ki *= ratio;
        _kd /= ratio;
        _sampleTime = ms;
    }

    void SetControllerDirection(int dir) { _direction = dir; }

    double GetKp() const { return _direction == DIRECT ? _kp : -_kp; }
    double GetKi() const {
        double ki = _direction == DIRECT ? _ki : -_ki;
        return ki / ((double)_sampleTime / 1000.0);
    }
    double GetKd() const {
        double kd = _direction == DIRECT ? _kd : -_kd;
        return kd * ((double)_sampleTime / 1000.0);
    }
    int GetMode() const { return _mode; }
    int GetDirection() const { return _direction; }

    // Test helpers
    uint32_t _getComputeCount() const { return _computeCount; }

private:
    double* _input;
    double* _output;
    double* _setpoint;
    double _kp, _ki, _kd;
    int _pOn;
    int _direction;
    int _mode = MANUAL;
    double _outMin, _outMax;
    int _sampleTime;
    double _outputSum;
    double _lastInput;
    unsigned long _lastTime;
    uint32_t _computeCount = 0;
};
