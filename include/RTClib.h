#pragma once
// esp32emu — RTClib mock (Adafruit RTClib for DS1307, DS3231, PCF8523)

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <ctime>

class DateTime {
public:
    DateTime(uint32_t t = 0) {
        if (t == 0) {
            yOff_ = 25; m_ = 1; d_ = 1; hh_ = 0; mm_ = 0; ss_ = 0;
        } else {
            time_t tt = (time_t)(t + 946684800UL); // Unix epoch offset from 2000
            struct tm* tm = gmtime(&tt);
            yOff_ = tm->tm_year - 100;
            m_ = tm->tm_mon + 1;
            d_ = tm->tm_mday;
            hh_ = tm->tm_hour;
            mm_ = tm->tm_min;
            ss_ = tm->tm_sec;
        }
    }

    DateTime(uint16_t year, uint8_t month, uint8_t day,
             uint8_t hour = 0, uint8_t min = 0, uint8_t sec = 0)
        : yOff_(year >= 2000 ? year - 2000 : year), m_(month), d_(day),
          hh_(hour), mm_(min), ss_(sec) {}

    DateTime(const char* date, const char* time) {
        // Parse __DATE__ format: "Feb 23 2026"
        static const char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
        char mon[4] = {};
        int day, year, hour, minute, second;
        sscanf(date, "%3s %d %d", mon, &day, &year);
        sscanf(time, "%d:%d:%d", &hour, &minute, &second);
        const char* p = strstr(months, mon);
        m_ = p ? (uint8_t)((p - months) / 3 + 1) : 1;
        yOff_ = (uint8_t)(year - 2000);
        d_ = (uint8_t)day;
        hh_ = (uint8_t)hour;
        mm_ = (uint8_t)minute;
        ss_ = (uint8_t)second;
    }

    uint16_t year() const { return yOff_ + 2000; }
    uint8_t month() const { return m_; }
    uint8_t day() const { return d_; }
    uint8_t hour() const { return hh_; }
    uint8_t minute() const { return mm_; }
    uint8_t second() const { return ss_; }

    uint8_t dayOfTheWeek() const {
        // Tomohiko Sakamoto's algorithm
        static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
        int y = year();
        int m = m_;
        if (m < 3) y--;
        return (y + y/4 - y/100 + y/400 + t[m-1] + d_) % 7;
    }

    uint32_t unixtime() const {
        struct tm t = {};
        t.tm_year = yOff_ + 100;
        t.tm_mon = m_ - 1;
        t.tm_mday = d_;
        t.tm_hour = hh_;
        t.tm_min = mm_;
        t.tm_sec = ss_;
        t.tm_isdst = 0;
        // Use timegm-equivalent
        time_t epoch = mktime(&t);
        // Adjust for local timezone offset
        struct tm* gm = gmtime(&epoch);
        time_t utc = mktime(gm);
        epoch += (epoch - utc);
        return (uint32_t)epoch;
    }

    uint32_t secondstime() const {
        // Seconds since 2000-01-01
        return unixtime() - 946684800UL;
    }

    DateTime operator+(uint32_t seconds) const {
        return DateTime(secondstime() + seconds);
    }

    bool operator==(const DateTime& o) const {
        return yOff_ == o.yOff_ && m_ == o.m_ && d_ == o.d_ &&
               hh_ == o.hh_ && mm_ == o.mm_ && ss_ == o.ss_;
    }
    bool operator!=(const DateTime& o) const { return !(*this == o); }
    bool operator<(const DateTime& o) const { return unixtime() < o.unixtime(); }
    bool operator>(const DateTime& o) const { return unixtime() > o.unixtime(); }

private:
    uint8_t yOff_ = 25, m_ = 1, d_ = 1, hh_ = 0, mm_ = 0, ss_ = 0;
};

class TimeSpan {
public:
    TimeSpan(int32_t seconds = 0) : totalseconds_(seconds) {}
    TimeSpan(int16_t days, int8_t hours, int8_t minutes, int8_t seconds)
        : totalseconds_((int32_t)days * 86400L + hours * 3600 + minutes * 60 + seconds) {}

    int16_t days() const { return totalseconds_ / 86400L; }
    int8_t hours() const { return (totalseconds_ % 86400L) / 3600; }
    int8_t minutes() const { return (totalseconds_ % 3600) / 60; }
    int8_t seconds() const { return totalseconds_ % 60; }
    int32_t totalseconds() const { return totalseconds_; }

private:
    int32_t totalseconds_;
};

// Base RTC class
class RTC_Base {
public:
    bool begin(void* /*wire*/ = nullptr) {
        initialized_ = true;
        return true;
    }

    bool initialized() const { return initialized_; }

    void adjust(const DateTime& dt) {
        now_ = dt;
        fprintf(stderr, "[esp32emu] RTC: time set to %04d-%02d-%02d %02d:%02d:%02d\n",
                dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second());
    }

    DateTime now() const { return now_; }

    // Test helpers
    void test_setNow(const DateTime& dt) { now_ = dt; }

protected:
    bool initialized_ = false;
    DateTime now_;
};

class RTC_DS1307 : public RTC_Base {
public:
    bool begin(void* wire = nullptr) {
        fprintf(stderr, "[esp32emu] RTC_DS1307: initialized\n");
        return RTC_Base::begin(wire);
    }

    bool isrunning() const { return running_; }
    void test_setRunning(bool r) { running_ = r; }

    // Square-wave output
    enum Sqw { OFF = 0, ON = 1, SquareWave1HZ = 2, SquareWave4kHz = 3,
               SquareWave8kHz = 4, SquareWave32kHz = 5 };
    void writeSqwPinMode(Sqw mode) { sqw_ = mode; }
    Sqw readSqwPinMode() const { return sqw_; }

    // NVRAM (56 bytes)
    uint8_t readnvram(uint8_t addr) { return (addr < 56) ? nvram_[addr] : 0; }
    void writenvram(uint8_t addr, uint8_t val) { if (addr < 56) nvram_[addr] = val; }

private:
    bool running_ = true;
    Sqw sqw_ = OFF;
    uint8_t nvram_[56] = {};
};

class RTC_DS3231 : public RTC_Base {
public:
    bool begin(void* wire = nullptr) {
        fprintf(stderr, "[esp32emu] RTC_DS3231: initialized\n");
        return RTC_Base::begin(wire);
    }

    bool lostPower() const { return lostPower_; }
    void test_setLostPower(bool lp) { lostPower_ = lp; }

    float getTemperature() const { return temperature_; }
    void test_setTemperature(float t) { temperature_ = t; }

    enum Sqw { DS3231_OFF = 0, DS3231_SquareWave1Hz = 1,
               DS3231_SquareWave1kHz = 2, DS3231_SquareWave4kHz = 3,
               DS3231_SquareWave8kHz = 4 };
    void writeSqwPinMode(Sqw mode) { sqw_ = mode; }
    Sqw readSqwPinMode() const { return sqw_; }

private:
    bool lostPower_ = false;
    float temperature_ = 25.0f;
    Sqw sqw_ = DS3231_OFF;
};

class RTC_PCF8523 : public RTC_Base {
public:
    bool begin(void* wire = nullptr) {
        fprintf(stderr, "[esp32emu] RTC_PCF8523: initialized\n");
        return RTC_Base::begin(wire);
    }

    bool lostPower() const { return lostPower_; }
    void test_setLostPower(bool lp) { lostPower_ = lp; }

    bool initialized() const { return RTC_Base::initialized(); }

    // Calibration
    void calibrate(int8_t mode, int8_t offset) { calMode_ = mode; calOffset_ = offset; }

private:
    bool lostPower_ = false;
    int8_t calMode_ = 0, calOffset_ = 0;
};
