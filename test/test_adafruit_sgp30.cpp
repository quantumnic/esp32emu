#include "Adafruit_SGP30.h"
#include <cassert>
#include <cstdio>

int main() {
    Adafruit_SGP30 sgp;
    assert(!sgp.emu_begun());

    assert(sgp.begin());
    assert(sgp.emu_begun());

    // IAQ init
    assert(sgp.IAQinit());
    assert(sgp.emu_iaqInit());

    // Default values
    assert(sgp.IAQmeasure());
    assert(sgp.eCO2 == 400);
    assert(sgp.TVOC == 0);
    assert(sgp.emu_measureCount() == 1);

    // Inject values
    sgp.emu_seteCO2(1200);
    sgp.emu_setTVOC(350);
    assert(sgp.IAQmeasure());
    assert(sgp.eCO2 == 1200);
    assert(sgp.TVOC == 350);
    assert(sgp.emu_measureCount() == 2);

    // Raw measurements
    sgp.emu_setRawH2(14000);
    sgp.emu_setRawEthanol(20000);
    assert(sgp.IAQmeasureRaw());
    assert(sgp.rawH2 == 14000);
    assert(sgp.rawEthanol == 20000);

    // Baseline get/set
    uint16_t eco2_base, tvoc_base;
    sgp.setIAQBaseline(0x1234, 0x5678);
    assert(sgp.getIAQBaseline(&eco2_base, &tvoc_base));
    assert(eco2_base == 0x1234);
    assert(tvoc_base == 0x5678);

    // Humidity compensation
    assert(sgp.setHumidity(13000));
    assert(sgp.emu_absHumidity() == 13000);

    // Serial number accessible
    assert(sgp.serialnumber[1] == 0x0048);

    printf("test_adafruit_sgp30: all assertions passed\n");
    return 0;
}
