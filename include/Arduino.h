#pragma once
// esp32emu — Lightweight ESP32/Arduino emulator for host-side testing

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <thread>
#include <string>
#include <cmath>
#include <functional>

typedef uint8_t byte;
typedef bool boolean;

#define INPUT           0x0
#define OUTPUT          0x1
#define INPUT_PULLUP    0x2
#define INPUT_PULLDOWN  0x3

#define HIGH 1
#define LOW  0

#define LED_BUILTIN 2

#define A0 36
#define A1 37
#define A2 38
#define A3 39

#ifndef PI
#define PI 3.14159265358979323846
#endif
#define DEG_TO_RAD 0.017453292519943295
#define RAD_TO_DEG 57.29577951308232

#define constrain(x,lo,hi) ((x)<(lo)?(lo):((x)>(hi)?(hi):(x)))

inline long arduino_map(long x,long in_min,long in_max,long out_min,long out_max){
    return (x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min;
}
#define map arduino_map

#include "esp32emu_gpio.h"
#include "esp32emu_time.h"
#include "esp32emu_serial.h"
#include "esp32emu_string.h"

inline long random(long mx){return std::rand()%mx;}
inline long random(long mn,long mx){return mn+std::rand()%(mx-mn);}
inline void randomSeed(unsigned long s){std::srand(s);}
inline void yield(){std::this_thread::yield();}
