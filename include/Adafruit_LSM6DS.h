// Adafruit_LSM6DS.h — LSM6DS family (LSM6DSO/DSOX/DS33) 6-axis IMU mock for esp32emu
#pragma once
#include "Adafruit_Sensor.h"
#include <cstdint>
#include <cstdio>
#include <cmath>

// Data rate
typedef enum {
    LSM6DS_RATE_SHUTDOWN = 0,
    LSM6DS_RATE_12_5_HZ,
    LSM6DS_RATE_26_HZ,
    LSM6DS_RATE_52_HZ,
    LSM6DS_RATE_104_HZ,
    LSM6DS_RATE_208_HZ,
    LSM6DS_RATE_416_HZ,
    LSM6DS_RATE_833_HZ,
    LSM6DS_RATE_1_66K_HZ,
    LSM6DS_RATE_3_33K_HZ,
    LSM6DS_RATE_6_66K_HZ,
} lsm6ds_data_rate_t;

// Accel range
typedef enum {
    LSM6DS_ACCEL_RANGE_2_G = 0,
    LSM6DS_ACCEL_RANGE_4_G,
    LSM6DS_ACCEL_RANGE_8_G,
    LSM6DS_ACCEL_RANGE_16_G,
} lsm6ds_accel_range_t;

// Gyro range
typedef enum {
    LSM6DS_GYRO_RANGE_125_DPS = 0,
    LSM6DS_GYRO_RANGE_250_DPS,
    LSM6DS_GYRO_RANGE_500_DPS,
    LSM6DS_GYRO_RANGE_1000_DPS,
    LSM6DS_GYRO_RANGE_2000_DPS,
} lsm6ds_gyro_range_t;

namespace lsm6ds_test {
    struct Vec3 { float x, y, z; };
    inline Vec3& accel()    { static Vec3 v = {0.0f, 0.0f, 9.81f}; return v; }
    inline Vec3& gyro()     { static Vec3 v = {0.0f, 0.0f, 0.0f};  return v; }
    inline float& temp()    { static float v = 25.0f; return v; }
    inline bool& available() { static bool v = true; return v; }
    inline void reset() {
        accel() = {0.0f, 0.0f, 9.81f};
        gyro() = {0.0f, 0.0f, 0.0f};
        temp() = 25.0f;
        available() = true;
    }
    inline void setAccel(float x, float y, float z) { accel() = {x, y, z}; }
    inline void setGyro(float x, float y, float z) { gyro() = {x, y, z}; }
    inline void setTemperature(float t) { temp() = t; }
}

class Adafruit_LSM6DS {
public:
    Adafruit_LSM6DS() : _begun(false), _accel_range(LSM6DS_ACCEL_RANGE_4_G),
        _gyro_range(LSM6DS_GYRO_RANGE_250_DPS), _data_rate(LSM6DS_RATE_104_HZ) {}
    virtual ~Adafruit_LSM6DS() = default;

    bool begin_I2C(uint8_t addr = 0x6A) {
        (void)addr;
        if (!lsm6ds_test::available()) return false;
        _begun = true;
        printf("[esp32emu] LSM6DS: initialized (I2C 0x%02X)\n", addr);
        return true;
    }

    bool begin_SPI(uint8_t cs) {
        (void)cs;
        if (!lsm6ds_test::available()) return false;
        _begun = true;
        printf("[esp32emu] LSM6DS: initialized (SPI CS=%u)\n", cs);
        return true;
    }

    bool getEvent(sensors_event_t* accel, sensors_event_t* gyro, sensors_event_t* temp) {
        if (!_begun) return false;
        if (accel) {
            accel->acceleration.x = lsm6ds_test::accel().x;
            accel->acceleration.y = lsm6ds_test::accel().y;
            accel->acceleration.z = lsm6ds_test::accel().z;
        }
        if (gyro) {
            gyro->gyro.x = lsm6ds_test::gyro().x;
            gyro->gyro.y = lsm6ds_test::gyro().y;
            gyro->gyro.z = lsm6ds_test::gyro().z;
        }
        if (temp) {
            temp->temperature = lsm6ds_test::temp();
        }
        return true;
    }

    void setAccelRange(lsm6ds_accel_range_t r) { _accel_range = r; }
    lsm6ds_accel_range_t getAccelRange() const { return _accel_range; }

    void setGyroRange(lsm6ds_gyro_range_t r) { _gyro_range = r; }
    lsm6ds_gyro_range_t getGyroRange() const { return _gyro_range; }

    void setAccelDataRate(lsm6ds_data_rate_t r) { _data_rate = r; }
    lsm6ds_data_rate_t getAccelDataRate() const { return _data_rate; }

    void setGyroDataRate(lsm6ds_data_rate_t r) { _data_rate = r; }

protected:
    bool _begun;
    lsm6ds_accel_range_t _accel_range;
    lsm6ds_gyro_range_t _gyro_range;
    lsm6ds_data_rate_t _data_rate;
};

class Adafruit_LSM6DSO : public Adafruit_LSM6DS {};
class Adafruit_LSM6DSOX : public Adafruit_LSM6DS {};
class Adafruit_LSM6DS33 : public Adafruit_LSM6DS {};
