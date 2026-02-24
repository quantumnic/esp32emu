#pragma once
#include <cstdint>
#include <cstdio>

// LIS3MDL 3-axis magnetometer mock
typedef enum {
    LIS3MDL_RANGE_4_GAUSS = 0,
    LIS3MDL_RANGE_8_GAUSS,
    LIS3MDL_RANGE_12_GAUSS,
    LIS3MDL_RANGE_16_GAUSS
} lis3mdl_range_t;

typedef enum {
    LIS3MDL_DATARATE_0_625_HZ = 0,
    LIS3MDL_DATARATE_1_25_HZ,
    LIS3MDL_DATARATE_2_5_HZ,
    LIS3MDL_DATARATE_5_HZ,
    LIS3MDL_DATARATE_10_HZ,
    LIS3MDL_DATARATE_20_HZ,
    LIS3MDL_DATARATE_40_HZ,
    LIS3MDL_DATARATE_80_HZ,
    LIS3MDL_DATARATE_155_HZ,
    LIS3MDL_DATARATE_300_HZ,
    LIS3MDL_DATARATE_560_HZ,
    LIS3MDL_DATARATE_1000_HZ
} lis3mdl_dataRate_t;

typedef enum {
    LIS3MDL_CONTINUOUSMODE = 0,
    LIS3MDL_SINGLEMODE,
    LIS3MDL_POWERDOWNMODE
} lis3mdl_operationmode_t;

typedef enum {
    LIS3MDL_LOWPOWERMODE = 0,
    LIS3MDL_MEDIUMMODE,
    LIS3MDL_HIGHMODE,
    LIS3MDL_ULTRAHIGHMODE
} lis3mdl_performancemode_t;

class Adafruit_LIS3MDL {
public:
    Adafruit_LIS3MDL() = default;

    bool begin_I2C(uint8_t addr = 0x1C, TwoWire* wire = nullptr) {
        (void)addr; (void)wire;
        started_ = true;
        return true;
    }

    bool begin_SPI(uint8_t cs) { (void)cs; started_ = true; return true; }

    void read() { /* data already set via test helpers */ }

    void setRange(lis3mdl_range_t range) { range_ = range; }
    lis3mdl_range_t getRange() { return range_; }

    void setDataRate(lis3mdl_dataRate_t rate) { data_rate_ = rate; }
    lis3mdl_dataRate_t getDataRate() { return data_rate_; }

    void setOperationMode(lis3mdl_operationmode_t mode) { op_mode_ = mode; }
    lis3mdl_operationmode_t getOperationMode() { return op_mode_; }

    void setPerformanceMode(lis3mdl_performancemode_t mode) { perf_mode_ = mode; }
    lis3mdl_performancemode_t getPerformanceMode() { return perf_mode_; }

    void setIntThreshold(uint16_t threshold) { int_threshold_ = threshold; }
    uint16_t getIntThreshold() { return int_threshold_; }

    void configInterrupt(bool enableX, bool enableY, bool enableZ, bool polarity,
                         bool latch, bool enableInt) {
        int_x_ = enableX; int_y_ = enableY; int_z_ = enableZ;
        int_polarity_ = polarity; int_latch_ = latch; int_enabled_ = enableInt;
    }

    bool selfTest() { return true; }

    // Raw data fields (public like Adafruit library)
    int16_t x = 0, y = 0, z = 0;
    float x_gauss = 0.0f, y_gauss = 0.0f, z_gauss = 0.0f;

    // Test helpers
    void test_setMag(float gx, float gy, float gz) {
        x_gauss = gx; y_gauss = gy; z_gauss = gz;
        x = (int16_t)(gx * 6842); y = (int16_t)(gy * 6842); z = (int16_t)(gz * 6842);
    }
    bool test_isStarted() const { return started_; }
    bool test_isIntEnabled() const { return int_enabled_; }

private:
    bool started_ = false;
    lis3mdl_range_t range_ = LIS3MDL_RANGE_4_GAUSS;
    lis3mdl_dataRate_t data_rate_ = LIS3MDL_DATARATE_10_HZ;
    lis3mdl_operationmode_t op_mode_ = LIS3MDL_CONTINUOUSMODE;
    lis3mdl_performancemode_t perf_mode_ = LIS3MDL_ULTRAHIGHMODE;
    uint16_t int_threshold_ = 0;
    bool int_x_ = false, int_y_ = false, int_z_ = false;
    bool int_polarity_ = false, int_latch_ = false, int_enabled_ = false;
};
