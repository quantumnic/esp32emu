#pragma once
#include <cstdio>
#include <cstdint>
#include <cstdarg>
#include <string>
#include <cstring>

class HardwareSerial {
public:
    void begin(unsigned long baud) { (void)baud; }
    void end() {}

    size_t print(const char* s)        { return fprintf(stdout, "%s", s); }
    size_t print(const std::string& s) { return fprintf(stdout, "%s", s.c_str()); }
    size_t print(char c)               { return fprintf(stdout, "%c", c); }
    size_t print(int n, int base = 10) {
        if (base == 16) return fprintf(stdout, "%x", n);
        if (base == 8)  return fprintf(stdout, "%o", n);
        if (base == 2) {
            if (n == 0) { putchar('0'); return 1; }
            char buf[33]; int i = 0;
            unsigned u = (unsigned)n;
            while (u) { buf[i++] = '0' + (u & 1); u >>= 1; }
            for (int j = i - 1; j >= 0; j--) putchar(buf[j]);
            return i;
        }
        return fprintf(stdout, "%d", n);
    }
    size_t print(unsigned int n, int base = 10) { return print((int)n, base); }
    size_t print(long n)               { return fprintf(stdout, "%ld", n); }
    size_t print(unsigned long n)      { return fprintf(stdout, "%lu", n); }
    size_t print(double n, int dec = 2){ return fprintf(stdout, "%.*f", dec, n); }
    size_t print(float n, int dec = 2) { return print((double)n, dec); }

    size_t println()                     { return fprintf(stdout, "\n"); }
    size_t println(const char* s)        { return fprintf(stdout, "%s\n", s); }
    size_t println(const std::string& s) { return fprintf(stdout, "%s\n", s.c_str()); }
    size_t println(char c)               { return fprintf(stdout, "%c\n", c); }
    size_t println(int n, int base = 10) { print(n, base); return fprintf(stdout, "\n"); }
    size_t println(unsigned int n, int b = 10) { print(n, b); return fprintf(stdout, "\n"); }
    size_t println(long n)               { return fprintf(stdout, "%ld\n", n); }
    size_t println(unsigned long n)      { return fprintf(stdout, "%lu\n", n); }
    size_t println(double n, int d = 2)  { print(n, d); return fprintf(stdout, "\n"); }
    size_t println(float n, int d = 2)   { return println((double)n, d); }

    size_t write(uint8_t c)              { return fputc(c, stdout) != EOF ? 1 : 0; }
    size_t write(const uint8_t* buf, size_t sz) { return fwrite(buf, 1, sz, stdout); }
    size_t write(const char* s)          { return fprintf(stdout, "%s", s); }

    int available() { return 0; }
    int read()      { return -1; }
    int peek()      { return -1; }
    void flush()    { fflush(stdout); }

    operator bool() const { return true; }

    size_t printf(const char* fmt, ...) __attribute__((format(printf, 2, 3))) {
        va_list args;
        va_start(args, fmt);
        size_t r = vfprintf(stdout, fmt, args);
        va_end(args);
        return r;
    }
};

extern HardwareSerial Serial;
extern HardwareSerial Serial1;
extern HardwareSerial Serial2;
