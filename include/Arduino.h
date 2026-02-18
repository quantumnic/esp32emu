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

// --- ESP32 LEDC PWM mock ---
inline double ledcSetup(uint8_t channel, double freq, uint8_t resolution_bits) {
    (void)channel; (void)resolution_bits;
    return freq;
}
inline void ledcAttachPin(uint8_t pin, uint8_t channel) {
    (void)pin; (void)channel;
}
inline void ledcDetachPin(uint8_t pin) { (void)pin; }
inline void ledcWrite(uint8_t channel, uint32_t duty) {
    (void)channel;
    esp32emu::GPIOController::instance().analogWrite(channel, duty);
}
inline uint32_t ledcRead(uint8_t channel) {
    return esp32emu::GPIOController::instance().analogRead(channel);
}
inline double ledcReadFreq(uint8_t channel) { (void)channel; return 5000.0; }
inline void ledcWriteTone(uint8_t channel, double freq) {
    (void)channel; (void)freq;
}
inline void ledcWriteNote(uint8_t channel, int note, int octave) {
    (void)channel; (void)note; (void)octave;
}

// --- tone / noTone ---
inline void tone(uint8_t pin, unsigned int frequency, unsigned long duration = 0) {
    fprintf(stderr, "🔊 tone(pin=%d, freq=%uHz", pin, frequency);
    if (duration > 0) fprintf(stderr, ", dur=%lums", duration);
    fprintf(stderr, ")\n");
    (void)pin; (void)frequency; (void)duration;
}
inline void noTone(uint8_t pin) {
    fprintf(stderr, "🔇 noTone(pin=%d)\n", pin);
    (void)pin;
}

// --- pulseIn ---
inline unsigned long pulseIn(uint8_t pin, uint8_t value, unsigned long timeout = 1000000UL) {
    (void)pin; (void)value; (void)timeout;
    // Return a simulated pulse width (e.g., ~1500us for ultrasonic ~25cm)
    return 1500;
}

// --- shiftOut / shiftIn ---
#define MSBFIRST 1
#define LSBFIRST 0
inline void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val) {
    (void)dataPin; (void)clockPin; (void)bitOrder; (void)val;
}
inline uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder) {
    (void)dataPin; (void)clockPin; (void)bitOrder;
    return 0;
}

// Arduino word/byte helpers
inline uint16_t word(uint8_t high, uint8_t low) { return (uint16_t(high) << 8) | low; }
inline uint8_t highByte(uint16_t w) { return uint8_t(w >> 8); }
inline uint8_t lowByte(uint16_t w) { return uint8_t(w & 0xFF); }
