#include "TinyGPSPlus.h"
#include <cassert>
#include <cstdio>
#include <cmath>
#include <cstring>

void feedString(TinyGPSPlus& gps, const char* s) {
    for (const char* p = s; *p; p++) gps.encode(*p);
}

int main() {
    // Test 1: Initial state
    {
        TinyGPSPlus gps;
        assert(!gps.location.isValid());
        assert(!gps.date.isValid());
        assert(!gps.time.isValid());
        assert(gps.charsProcessed() == 0);
    }

    // Test 2: Parse GGA sentence
    {
        TinyGPSPlus gps;
        feedString(gps, "$GPGGA,123519.00,4807.038,N,01131.000,E,1,08,0.9,545.4,M,47.0,M,,*47\r\n");
        assert(gps.location.isValid());
        assert(fabs(gps.location.lat() - 48.1173) < 0.001);
        assert(fabs(gps.location.lng() - 11.5167) < 0.001);
        assert(gps.satellites.isValid());
        assert(gps.satellites.value() == 8);
        assert(fabs(gps.altitude.meters() - 545.4) < 0.1);
        assert(gps.time.isValid());
        assert(gps.time.hour() == 12);
        assert(gps.time.minute() == 35);
        assert(gps.time.second() == 19);
    }

    // Test 3: Parse RMC sentence
    {
        TinyGPSPlus gps;
        feedString(gps, "$GPRMC,123519.00,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A\r\n");
        assert(gps.location.isValid());
        assert(fabs(gps.speed.knots() - 22.4) < 0.1);
        assert(fabs(gps.speed.kmph() - 22.4 * 1.852) < 0.1);
        assert(fabs(gps.course.deg() - 84.4) < 0.1);
        assert(gps.date.isValid());
        assert(gps.date.day() == 23);
        assert(gps.date.month() == 3);
        assert(gps.date.year() == 2094);
    }

    // Test 4: Southern hemisphere
    {
        TinyGPSPlus gps;
        feedString(gps, "$GPGGA,000000.00,3352.000,S,15113.000,W,1,04,1.0,100.0,M,0.0,M,,*00\r\n");
        assert(gps.location.lat() < 0);
        assert(gps.location.lng() < 0);
    }

    // Test 5: Distance and course calculations
    {
        double d = TinyGPSPlus::distanceBetween(0.0, 0.0, 0.0, 1.0);
        assert(d > 110000 && d < 112000); // ~111km per degree at equator

        double c = TinyGPSPlus::courseTo(0.0, 0.0, 1.0, 0.0);
        assert(fabs(c - 0.0) < 1.0); // due north

        c = TinyGPSPlus::courseTo(0.0, 0.0, 0.0, 1.0);
        assert(fabs(c - 90.0) < 1.0); // due east
    }

    // Test 6: Test helpers
    {
        TinyGPSPlus gps;
        gps.setLocation(47.3769, 8.5417);
        assert(gps.location.isValid());
        assert(fabs(gps.location.lat() - 47.3769) < 0.001);

        gps.setSpeed(50.0);
        assert(fabs(gps.speed.kmph() - 92.6) < 0.1);

        gps.setAltitude(408.0);
        assert(fabs(gps.altitude.feet() - 408.0 * 3.28084) < 1.0);

        gps.setSatellites(12);
        assert(gps.satellites.value() == 12);
    }

    // Test 7: Altitude conversions
    {
        TinyGPSPlus gps;
        gps.setAltitude(1000.0);
        assert(fabs(gps.altitude.kilometers() - 1.0) < 0.001);
        assert(fabs(gps.altitude.miles() - 0.621371) < 0.001);
    }

    // Test 8: Multiple sentences
    {
        TinyGPSPlus gps;
        feedString(gps, "$GPGGA,100000.00,4000.000,N,00800.000,E,1,06,1.2,300.0,M,0.0,M,,*00\r\n");
        feedString(gps, "$GPRMC,100000.00,A,4000.000,N,00800.000,E,010.0,270.0,150226,000.0,E*00\r\n");
        assert(gps.sentencesWithFix() == 2);
        assert(fabs(gps.speed.knots() - 10.0) < 0.1);
        assert(gps.satellites.value() == 6);
    }

    printf("test_tinygpsplus: all assertions passed\n");
    return 0;
}
