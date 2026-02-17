#include "Wire.h"
#include <cassert>
#include <cstdio>

int main() {
    Wire.begin();

    // Register a mock I2C temperature sensor at 0x48
    Wire.addDevice(0x48, [](const std::vector<uint8_t>& tx, uint8_t qty) -> std::vector<uint8_t> {
        (void)tx;
        std::vector<uint8_t> resp(qty, 0);
        if (qty >= 2) {
            resp[0] = 0x00; // 25.0°C → MSB
            resp[1] = 0xC8; // LSB (25.0 * 8 = 200 = 0x00C8)
        }
        return resp;
    });

    // Read from the mock device
    Wire.beginTransmission(0x48);
    Wire.write(0x00); // register address
    Wire.endTransmission();

    uint8_t count = Wire.requestFrom((uint8_t)0x48, (uint8_t)2);
    assert(count == 2);
    assert(Wire.available() == 2);

    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();
    assert(msb == 0x00);
    assert(lsb == 0xC8);
    assert(Wire.available() == 0);

    Wire.removeDevice(0x48);

    printf("test_wire: all assertions passed\n");
    return 0;
}
