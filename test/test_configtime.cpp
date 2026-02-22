#include "esp32emu_time.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // configTime
    configTime(3600, 3600, "pool.ntp.org", "time.nist.gov");
    auto& tc = esp32emu::TimeConfig::instance();
    assert(tc.configured);
    assert(tc.gmt_offset_sec == 3600);
    assert(tc.daylight_offset_sec == 3600);
    assert(tc.ntp_server1 == "pool.ntp.org");
    assert(tc.ntp_server2 == "time.nist.gov");

    // getLocalTime
    struct tm timeinfo;
    assert(getLocalTime(&timeinfo));
    assert(timeinfo.tm_year >= 125); // 2025+
    assert(timeinfo.tm_mon >= 0 && timeinfo.tm_mon <= 11);
    assert(timeinfo.tm_mday >= 1 && timeinfo.tm_mday <= 31);

    // configTzTime
    configTzTime("CET-1CEST,M3.5.0,M10.5.0/3", "pool.ntp.org");

    // Reset
    tc.reset();
    assert(!tc.configured);

    printf("test_configtime: all assertions passed\n");
    return 0;
}
