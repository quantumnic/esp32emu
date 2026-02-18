#include "SPI.h"
#include <cassert>
#include <cstdio>

SPIClass SPI;

int main() {
    SPI.begin();

    SPISettings settings(1000000, MSBFIRST, SPI_MODE0);
    SPI.beginTransaction(settings);

    // Echo mode: transfer returns same byte
    assert(SPI.transfer(0x42) == 0x42);
    assert(SPI.transfer(0xFF) == 0xFF);
    assert(SPI.transfer(0x00) == 0x00);

    // transfer16 echo
    assert(SPI.transfer16(0x1234) == 0x1234);

    SPI.endTransaction();
    SPI.end();

    printf("test_spi: all assertions passed\n");
    return 0;
}
