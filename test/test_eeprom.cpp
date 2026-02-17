#include "EEPROM.h"
#include <cassert>
#include <cstdio>

int main() {
    EEPROM.begin(512);

    // Write and read
    EEPROM.write(0, 0xAB);
    EEPROM.write(1, 0xCD);
    assert(EEPROM.read(0) == 0xAB);
    assert(EEPROM.read(1) == 0xCD);

    // Struct put/get
    struct Config { int magic; float value; };
    Config w = {12345, 3.14f};
    EEPROM.put(100, w);

    Config r{};
    EEPROM.get(100, r);
    assert(r.magic == 12345);
    assert(r.value > 3.13f && r.value < 3.15f);

    // Out of bounds
    assert(EEPROM.read(999) == 0);

    EEPROM.commit();

    printf("test_eeprom: all assertions passed\n");
    return 0;
}
