#pragma once
#include <cstdint>
#include <cstddef>

#define SPI_MODE0 0
#define SPI_MODE1 1
#define SPI_MODE2 2
#define SPI_MODE3 3

#define MSBFIRST 1
#define LSBFIRST 0

class SPISettings {
public:
    SPISettings() {}
    SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode)
        : clock_(clock), bitOrder_(bitOrder), dataMode_(dataMode) {}
private:
    uint32_t clock_ = 1000000;
    uint8_t bitOrder_ = MSBFIRST;
    uint8_t dataMode_ = SPI_MODE0;
};

class SPIClass {
public:
    void begin(int8_t sck = -1, int8_t miso = -1, int8_t mosi = -1, int8_t ss = -1) {
        (void)sck; (void)miso; (void)mosi; (void)ss;
    }
    void end() {}
    void beginTransaction(SPISettings settings) { (void)settings; }
    void endTransaction() {}
    uint8_t transfer(uint8_t data) { return data; } // echo
    void transfer(void* buf, size_t count) { (void)buf; (void)count; }
    uint16_t transfer16(uint16_t data) { return data; }
    void setBitOrder(uint8_t bo) { (void)bo; }
    void setDataMode(uint8_t dm) { (void)dm; }
    void setClockDivider(uint8_t cd) { (void)cd; }
};

extern SPIClass SPI;
