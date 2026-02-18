/*
 * Touch Piano — ESP32 capacitive touch pin demo
 *
 * Uses touch pins T0-T6 as piano keys (C D E F G A B).
 * When a touch is detected (value drops below threshold),
 * the corresponding note frequency is played via tone().
 *
 * Board: ESP32 (has capacitive touch on GPIO 4,0,2,15,13,12,14,27,33,32)
 * Touch pin mapping: T0=GPIO4, T2=GPIO2, T3=GPIO15, T4=GPIO13, T5=GPIO12, T6=GPIO14, T7=GPIO27
 */

#include <Arduino.h>
#include "esp32emu_touch.h"

#define SPEAKER_PIN   25
#define TOUCH_THRESHOLD 30
#define NUM_KEYS 7

// Touch pins and their note frequencies (C major scale)
const uint8_t touch_pins[NUM_KEYS] = {4, 0, 2, 15, 13, 12, 14};
const char* note_names[NUM_KEYS] = {"C4", "D4", "E4", "F4", "G4", "A4", "B4"};
const int frequencies[NUM_KEYS] = {262, 294, 330, 349, 392, 440, 494};

void setup() {
    Serial.begin(115200);
    Serial.println("🎹 Touch Piano — Touch the pins to play notes!");
    Serial.println("Pins: T0(4)=C  T1(0)=D  T2(2)=E  T3(15)=F  T4(13)=G  T5(12)=A  T6(14)=B");
    Serial.println();
}

void loop() {
    bool any_touched = false;

    for (int i = 0; i < NUM_KEYS; i++) {
        uint16_t val = touchRead(touch_pins[i]);
        if (val < TOUCH_THRESHOLD) {
            Serial.printf("♪ %s (%dHz) — touch value: %d\n",
                         note_names[i], frequencies[i], val);
            tone(SPEAKER_PIN, frequencies[i], 200);
            any_touched = true;
            break;  // play only one note at a time
        }
    }

    if (!any_touched) {
        noTone(SPEAKER_PIN);
    }

    delay(50);
}
