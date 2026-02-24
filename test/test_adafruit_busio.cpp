#include "Adafruit_I2CDevice.h"
#include "Adafruit_SPIDevice.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Test 1: I2C init
    {
        Adafruit_I2CDevice dev(0x50);
        assert(!dev.detected());
        assert(dev.begin());
        assert(dev.detected());
        assert(dev.address() == 0x50);
    }

    // Test 2: I2C write
    {
        Adafruit_I2CDevice dev(0x50);
        dev.begin();
        uint8_t data[] = {0x01, 0x02, 0x03};
        assert(dev.write(data, 3));
        assert(dev.getWrittenLen() == 3);
        assert(memcmp(dev.getWrittenData(), data, 3) == 0);
    }

    // Test 3: I2C read with injected data
    {
        Adafruit_I2CDevice dev(0x50);
        dev.begin();
        uint8_t inject[] = {0xAA, 0xBB, 0xCC};
        dev.setReadData(inject, 3);
        uint8_t buf[3] = {};
        assert(dev.read(buf, 3));
        assert(buf[0] == 0xAA && buf[1] == 0xBB && buf[2] == 0xCC);
    }

    // Test 4: I2C write_then_read
    {
        Adafruit_I2CDevice dev(0x50);
        dev.begin();
        uint8_t inject[] = {0x42};
        dev.setReadData(inject, 1);
        uint8_t cmd = 0x01;
        uint8_t result = 0;
        assert(dev.write_then_read(&cmd, 1, &result, 1));
        assert(result == 0x42);
    }

    // Test 5: I2C write with prefix
    {
        Adafruit_I2CDevice dev(0x50);
        dev.begin();
        uint8_t prefix[] = {0x10};
        uint8_t data[] = {0x20, 0x30};
        assert(dev.write(data, 2, true, prefix, 1));
        assert(dev.getWrittenLen() == 3);
        assert(dev.getWrittenData()[0] == 0x10);
        assert(dev.getWrittenData()[1] == 0x20);
    }

    // Test 6: SPI init
    {
        Adafruit_SPIDevice dev(10); // CS=10
        assert(!dev.detected());
        assert(dev.begin());
        assert(dev.detected());
    }

    // Test 7: SPI write
    {
        Adafruit_SPIDevice dev(10);
        dev.begin();
        uint8_t data[] = {0xDE, 0xAD};
        assert(dev.write(data, 2));
        assert(dev.getWrittenLen() == 2);
    }

    // Test 8: SPI read
    {
        Adafruit_SPIDevice dev(10);
        dev.begin();
        uint8_t inject[] = {0xBE, 0xEF};
        dev.setReadData(inject, 2);
        uint8_t buf[2] = {};
        assert(dev.read(buf, 2));
        assert(buf[0] == 0xBE && buf[1] == 0xEF);
    }

    // Test 9: SPI write_then_read
    {
        Adafruit_SPIDevice dev(10);
        dev.begin();
        uint8_t inject[] = {0x55};
        dev.setReadData(inject, 1);
        uint8_t cmd = 0x00;
        uint8_t result = 0;
        assert(dev.write_then_read(&cmd, 1, &result, 1));
        assert(result == 0x55);
    }

    // Test 10: Software SPI constructor
    {
        Adafruit_SPIDevice dev((int8_t)10, (int8_t)11, (int8_t)12, (int8_t)13); // CS, SCK, MISO, MOSI
        assert(dev.begin());
        assert(dev.detected());
    }

    printf("test_adafruit_busio: all assertions passed\n");
    return 0;
}
