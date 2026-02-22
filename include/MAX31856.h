#pragma once
// esp32emu — MAX31856 precision thermocouple amplifier mock

#include <cstdint>
#include <cstdio>

typedef enum {
    MAX31856_TCTYPE_B  = 0b0000,
    MAX31856_TCTYPE_E  = 0b0001,
    MAX31856_TCTYPE_J  = 0b0010,
    MAX31856_TCTYPE_K  = 0b0011,
    MAX31856_TCTYPE_N  = 0b0100,
    MAX31856_TCTYPE_R  = 0b0101,
    MAX31856_TCTYPE_S  = 0b0110,
    MAX31856_TCTYPE_T  = 0b0111,
} max31856_thermocoupletype_t;

typedef enum {
    MAX31856_NOISE_FILTER_50HZ = 0,
    MAX31856_NOISE_FILTER_60HZ = 1,
} max31856_noise_filter_t;

typedef enum {
    MAX31856_OCFAULT_NONE    = 0b00,
    MAX31856_OCFAULT_NOINPUT = 0b01,
} max31856_ocfault_t;

class Adafruit_MAX31856 {
public:
    Adafruit_MAX31856(int8_t cs, int8_t mosi = -1, int8_t miso = -1, int8_t sck = -1)
        : cs_(cs), mosi_(mosi), miso_(miso), sck_(sck) {}

    bool begin() {
        initialized_ = true;
        fprintf(stderr, "[esp32emu] MAX31856: initialized (CS pin %d)\n", cs_);
        return true;
    }

    void setThermocoupleType(max31856_thermocoupletype_t type) { tc_type_ = type; }
    max31856_thermocoupletype_t getThermocoupleType() const { return tc_type_; }

    void setNoiseFilter(max31856_noise_filter_t f) { filter_ = f; }

    void setConversionMode(bool oneshot) { oneshot_ = oneshot; }

    void triggerOneShot() {
        if (oneshot_) conversion_complete_ = true;
    }

    bool conversionComplete() { return conversion_complete_; }

    float readThermocoupleTemperature() {
        conversion_complete_ = false;
        return tc_temperature_;
    }

    float readCJTemperature() { return cj_temperature_; }

    uint8_t readFault() { return fault_; }

    void setTempFaultThresholds(float low, float high) {
        fault_low_ = low; fault_high_ = high;
    }

    // Test helpers
    void esp32emu_set_tc_temperature(float t) { tc_temperature_ = t; conversion_complete_ = true; }
    void esp32emu_set_cj_temperature(float t) { cj_temperature_ = t; }
    void esp32emu_set_fault(uint8_t f) { fault_ = f; }

private:
    int8_t cs_, mosi_, miso_, sck_;
    bool initialized_ = false;
    bool oneshot_ = false;
    bool conversion_complete_ = false;
    max31856_thermocoupletype_t tc_type_ = MAX31856_TCTYPE_K;
    max31856_noise_filter_t filter_ = MAX31856_NOISE_FILTER_60HZ;
    float tc_temperature_ = 250.0f;
    float cj_temperature_ = 25.0f;
    float fault_low_ = 0.0f;
    float fault_high_ = 1600.0f;
    uint8_t fault_ = 0;
};
