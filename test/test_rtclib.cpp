#include "RTClib.h"
#include <cassert>
#include <cstdio>
#include <cmath>

int main() {
    // Test DateTime construction
    DateTime dt(2026, 2, 23, 4, 7, 30);
    assert(dt.year() == 2026);
    assert(dt.month() == 2);
    assert(dt.day() == 23);
    assert(dt.hour() == 4);
    assert(dt.minute() == 7);
    assert(dt.second() == 30);

    // Test day of week (Feb 23, 2026 is Monday = 1)
    assert(dt.dayOfTheWeek() == 1);

    // Test DateTime comparison
    DateTime dt2(2026, 2, 23, 5, 0, 0);
    assert(dt < dt2);
    assert(dt2 > dt);
    assert(dt != dt2);

    DateTime dt3(2026, 2, 23, 4, 7, 30);
    assert(dt == dt3);

    // Test TimeSpan
    TimeSpan ts(1, 2, 30, 15);
    assert(ts.days() == 1);
    assert(ts.hours() == 2);
    assert(ts.minutes() == 30);
    assert(ts.seconds() == 15);
    assert(ts.totalseconds() == 86400 + 7200 + 1800 + 15);

    // Test DS1307
    RTC_DS1307 ds1307;
    assert(ds1307.begin());
    assert(ds1307.isrunning());
    ds1307.adjust(DateTime(2026, 3, 15, 12, 0, 0));
    assert(ds1307.now().year() == 2026);
    assert(ds1307.now().month() == 3);
    assert(ds1307.now().day() == 15);

    // NVRAM
    ds1307.writenvram(0, 42);
    ds1307.writenvram(10, 99);
    assert(ds1307.readnvram(0) == 42);
    assert(ds1307.readnvram(10) == 99);
    assert(ds1307.readnvram(55) == 0);  // valid
    assert(ds1307.readnvram(56) == 0);  // out of range

    // Test DS3231
    RTC_DS3231 ds3231;
    assert(ds3231.begin());
    assert(!ds3231.lostPower());
    ds3231.test_setLostPower(true);
    assert(ds3231.lostPower());
    ds3231.test_setTemperature(23.5f);
    assert(fabs(ds3231.getTemperature() - 23.5f) < 0.01f);

    // Test PCF8523
    RTC_PCF8523 pcf;
    assert(pcf.begin());
    pcf.adjust(DateTime(2026, 12, 25, 0, 0, 0));
    assert(pcf.now().month() == 12);
    assert(pcf.now().day() == 25);

    printf("test_rtclib: all assertions passed\n");
    return 0;
}
