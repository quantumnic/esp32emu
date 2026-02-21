#pragma once
// esp32emu: TinyGPS++ mock — GPS NMEA parsing emulation
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <string>

struct TinyGPSLocation {
    bool valid_ = false;
    double lat_ = 0.0, lng_ = 0.0;
    uint32_t age_ = 0xFFFFFFFF;
    bool isValid() const { return valid_; }
    bool isUpdated() const { return valid_; }
    double lat() const { return lat_; }
    double lng() const { return lng_; }
    uint32_t age() const { return age_; }
};

struct TinyGPSDate {
    bool valid_ = false;
    uint32_t value_ = 0; // DDMMYY
    uint32_t age_ = 0xFFFFFFFF;
    bool isValid() const { return valid_; }
    uint16_t year() const { return 2000 + (value_ % 100); }
    uint8_t month() const { return (value_ / 100) % 100; }
    uint8_t day() const { return value_ / 10000; }
    uint32_t age() const { return age_; }
};

struct TinyGPSTime {
    bool valid_ = false;
    uint32_t value_ = 0; // HHMMSSCC
    uint32_t age_ = 0xFFFFFFFF;
    bool isValid() const { return valid_; }
    uint8_t hour() const { return value_ / 1000000; }
    uint8_t minute() const { return (value_ / 10000) % 100; }
    uint8_t second() const { return (value_ / 100) % 100; }
    uint8_t centisecond() const { return value_ % 100; }
    uint32_t age() const { return age_; }
};

struct TinyGPSSpeed {
    bool valid_ = false;
    double knots_ = 0.0;
    uint32_t age_ = 0xFFFFFFFF;
    bool isValid() const { return valid_; }
    double knots() const { return knots_; }
    double mph() const { return knots_ * 1.15078; }
    double mps() const { return knots_ * 0.514444; }
    double kmph() const { return knots_ * 1.852; }
    uint32_t age() const { return age_; }
};

struct TinyGPSCourse {
    bool valid_ = false;
    double deg_ = 0.0;
    uint32_t age_ = 0xFFFFFFFF;
    bool isValid() const { return valid_; }
    double deg() const { return deg_; }
    uint32_t age() const { return age_; }
};

struct TinyGPSAltitude {
    bool valid_ = false;
    double meters_ = 0.0;
    uint32_t age_ = 0xFFFFFFFF;
    bool isValid() const { return valid_; }
    double meters() const { return meters_; }
    double feet() const { return meters_ * 3.28084; }
    double kilometers() const { return meters_ / 1000.0; }
    double miles() const { return meters_ * 0.000621371; }
    uint32_t age() const { return age_; }
};

struct TinyGPSInteger {
    bool valid_ = false;
    uint32_t value_ = 0;
    uint32_t age_ = 0xFFFFFFFF;
    bool isValid() const { return valid_; }
    uint32_t value() const { return value_; }
    uint32_t age() const { return age_; }
};

struct TinyGPSDecimal {
    bool valid_ = false;
    int32_t value_ = 0; // value * 100
    uint32_t age_ = 0xFFFFFFFF;
    bool isValid() const { return valid_; }
    int32_t value() const { return value_; }
    uint32_t age() const { return age_; }
};

class TinyGPSPlus {
public:
    TinyGPSLocation location;
    TinyGPSDate date;
    TinyGPSTime time;
    TinyGPSSpeed speed;
    TinyGPSCourse course;
    TinyGPSAltitude altitude;
    TinyGPSInteger satellites;
    TinyGPSDecimal hdop;

    uint32_t charsProcessed_ = 0;
    uint32_t sentencesWithFix_ = 0;
    uint32_t failedChecksum_ = 0;

    bool encode(char c) {
        charsProcessed_++;
        if (c == '$') {
            sentencePos_ = 0;
            sentence_[0] = '\0';
            inSentence_ = true;
            return false;
        }
        if (!inSentence_) return false;
        if (c == '\r' || c == '\n') {
            if (sentencePos_ > 0) {
                sentence_[sentencePos_] = '\0';
                inSentence_ = false;
                return parseSentence_();
            }
            return false;
        }
        if (sentencePos_ < sizeof(sentence_) - 1) {
            sentence_[sentencePos_++] = c;
        }
        return false;
    }

    uint32_t charsProcessed() const { return charsProcessed_; }
    uint32_t sentencesWithFix() const { return sentencesWithFix_; }
    uint32_t failedChecksum() const { return failedChecksum_; }

    static double distanceBetween(double lat1, double lng1, double lat2, double lng2) {
        double dLat = radians_(lat2 - lat1);
        double dLng = radians_(lng2 - lng1);
        double a = sin_(dLat / 2) * sin_(dLat / 2) +
                   cos_(radians_(lat1)) * cos_(radians_(lat2)) *
                   sin_(dLng / 2) * sin_(dLng / 2);
        double c = 2.0 * atan2(sqrt_(a), sqrt_(1.0 - a));
        return 6371000.0 * c; // meters
    }

    static double courseTo(double lat1, double lng1, double lat2, double lng2) {
        double dLng = radians_(lng2 - lng1);
        double rlat1 = radians_(lat1), rlat2 = radians_(lat2);
        double y = sin_(dLng) * cos_(rlat2);
        double x = cos_(rlat1) * sin_(rlat2) - sin_(rlat1) * cos_(rlat2) * cos_(dLng);
        double brng = atan2(y, x);
        return fmod_(degrees_(brng) + 360.0, 360.0);
    }

    // Test helpers
    void setLocation(double lat, double lng, uint32_t age = 0) {
        location.valid_ = true; location.lat_ = lat; location.lng_ = lng; location.age_ = age;
    }
    void setDateTime(uint32_t dateVal, uint32_t timeVal, uint32_t age = 0) {
        date.valid_ = true; date.value_ = dateVal; date.age_ = age;
        time.valid_ = true; time.value_ = timeVal; time.age_ = age;
    }
    void setSpeed(double knots, uint32_t age = 0) {
        speed.valid_ = true; speed.knots_ = knots; speed.age_ = age;
    }
    void setAltitude(double meters, uint32_t age = 0) {
        altitude.valid_ = true; altitude.meters_ = meters; altitude.age_ = age;
    }
    void setSatellites(uint32_t n, uint32_t age = 0) {
        satellites.valid_ = true; satellites.value_ = n; satellites.age_ = age;
    }

private:
    char sentence_[128] = {};
    size_t sentencePos_ = 0;
    bool inSentence_ = false;

    // Minimal trig using <cmath>
    static double radians_(double d) { return d * 3.14159265358979323846 / 180.0; }
    static double degrees_(double r) { return r * 180.0 / 3.14159265358979323846; }
    static double sin_(double x);
    static double cos_(double x);
    static double sqrt_(double x);
    static double fmod_(double a, double b);

    bool parseSentence_() {
        // Strip checksum
        char* star = nullptr;
        for (size_t i = 0; i < sentencePos_; i++) {
            if (sentence_[i] == '*') { star = &sentence_[i]; break; }
        }
        if (star) *star = '\0';

        // Split fields
        char* fields[20] = {};
        int nFields = 0;
        fields[nFields++] = sentence_;
        for (char* p = sentence_; *p && nFields < 20; p++) {
            if (*p == ',') { *p = '\0'; fields[nFields++] = p + 1; }
        }
        if (nFields < 1) return false;

        if (strstr(fields[0], "GGA") && nFields >= 15) {
            return parseGGA_(fields, nFields);
        }
        if (strstr(fields[0], "RMC") && nFields >= 12) {
            return parseRMC_(fields, nFields);
        }
        return false;
    }

    double parseCoord_(const char* raw, const char* dir) {
        if (!raw || !raw[0]) return 0.0;
        double val = atof(raw);
        int deg = (int)(val / 100);
        double min = val - deg * 100;
        double result = deg + min / 60.0;
        if (dir && (dir[0] == 'S' || dir[0] == 'W')) result = -result;
        return result;
    }

    uint32_t parseTime_(const char* raw) {
        if (!raw || !raw[0]) return 0;
        // HHMMSS.ss → HHMMSSCC
        int h = 0, m = 0, s = 0, cs = 0;
        sscanf(raw, "%2d%2d%2d.%2d", &h, &m, &s, &cs);
        return h * 1000000 + m * 10000 + s * 100 + cs;
    }

    uint32_t parseDate_(const char* raw) {
        if (!raw || !raw[0]) return 0;
        int d = 0, m = 0, y = 0;
        sscanf(raw, "%2d%2d%2d", &d, &m, &y);
        return d * 10000 + m * 100 + y;
    }

    bool parseGGA_(char** f, int n) {
        (void)n;
        if (f[1][0]) { time.valid_ = true; time.value_ = parseTime_(f[1]); time.age_ = 0; }
        if (f[2][0]) {
            location.valid_ = true;
            location.lat_ = parseCoord_(f[2], f[3]);
            location.lng_ = parseCoord_(f[4], f[5]);
            location.age_ = 0;
        }
        if (f[7][0]) { satellites.valid_ = true; satellites.value_ = atoi(f[7]); satellites.age_ = 0; }
        if (f[8][0]) { hdop.valid_ = true; hdop.value_ = (int)(atof(f[8]) * 100); hdop.age_ = 0; }
        if (f[9][0]) { altitude.valid_ = true; altitude.meters_ = atof(f[9]); altitude.age_ = 0; }
        sentencesWithFix_++;
        return true;
    }

    bool parseRMC_(char** f, int n) {
        (void)n;
        if (f[1][0]) { time.valid_ = true; time.value_ = parseTime_(f[1]); time.age_ = 0; }
        if (f[3][0]) {
            location.valid_ = true;
            location.lat_ = parseCoord_(f[3], f[4]);
            location.lng_ = parseCoord_(f[5], f[6]);
            location.age_ = 0;
        }
        if (f[7][0]) { speed.valid_ = true; speed.knots_ = atof(f[7]); speed.age_ = 0; }
        if (f[8][0]) { course.valid_ = true; course.deg_ = atof(f[8]); course.age_ = 0; }
        if (f[9][0]) { date.valid_ = true; date.value_ = parseDate_(f[9]); date.age_ = 0; }
        sentencesWithFix_++;
        return true;
    }
};

// Inline math implementations (avoid <cmath> linking issues)
#include <cmath>
inline double TinyGPSPlus::sin_(double x) { return ::sin(x); }
inline double TinyGPSPlus::cos_(double x) { return ::cos(x); }
inline double TinyGPSPlus::sqrt_(double x) { return ::sqrt(x); }
inline double TinyGPSPlus::fmod_(double a, double b) { return ::fmod(a, b); }
