#pragma once
// esp32emu mock: ICM-20948 — 9-axis IMU (accel + gyro + magnetometer)
#include <cstdint>
#include <cstdio>

class ICM_20948_I2C {
public:
    enum ODR { ODR_1100_HZ = 0, ODR_200_HZ = 4, ODR_50_HZ = 8, ODR_10_HZ = 11 };
    enum AccelFS { gpm2 = 0, gpm4, gpm8, gpm16 };
    enum GyroFS { dps250 = 0, dps500, dps1000, dps2000 };

    struct Status { bool success = true; operator bool() const { return success; } };

    ICM_20948_I2C() = default;

    Status begin(void* wire = nullptr, bool addrBit = false) {
        (void)wire; (void)addrBit;
        _initialized = true;
        printf("[esp32emu] ICM-20948: initialized\n");
        return Status{true};
    }

    bool dataReady() const { return _initialized && _dataReady; }

    Status getAGMT() {
        _dataReady = false;
        return Status{true};
    }

    float accX() const { return _ax; }
    float accY() const { return _ay; }
    float accZ() const { return _az; }
    float gyrX() const { return _gx; }
    float gyrY() const { return _gy; }
    float gyrZ() const { return _gz; }
    float magX() const { return _mx; }
    float magY() const { return _my; }
    float magZ() const { return _mz; }
    float temp() const { return _temp; }

    Status setFullScale(AccelFS afs) { _afs = afs; return {true}; }
    Status setFullScale(GyroFS gfs) { _gfs = gfs; return {true}; }
    Status enableDLPF(bool accel, bool en) { (void)accel; (void)en; return {true}; }
    Status sleep(bool on) { _sleeping = on; return {true}; }
    Status lowPower(bool on) { _lowPower = on; return {true}; }

    // ── Test helpers ──
    void _test_setAccel(float x, float y, float z) { _ax=x; _ay=y; _az=z; _dataReady=true; }
    void _test_setGyro(float x, float y, float z) { _gx=x; _gy=y; _gz=z; _dataReady=true; }
    void _test_setMag(float x, float y, float z) { _mx=x; _my=y; _mz=z; _dataReady=true; }
    void _test_setTemp(float t) { _temp = t; _dataReady = true; }
    bool _test_isInitialized() const { return _initialized; }
    bool _test_isSleeping() const { return _sleeping; }

private:
    bool _initialized = false;
    bool _dataReady = false;
    bool _sleeping = false;
    bool _lowPower = false;
    AccelFS _afs = gpm2;
    GyroFS _gfs = dps250;
    float _ax=0, _ay=0, _az=9.81f;
    float _gx=0, _gy=0, _gz=0;
    float _mx=25.0f, _my=0, _mz=40.0f;
    float _temp = 25.0f;
};
