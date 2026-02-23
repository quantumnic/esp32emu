// MIDI Controller — send notes and CC from potentiometers
#include <Arduino.h>
#include <MIDI.h>

MIDI_CREATE_DEFAULT_INSTANCE()

const int POT_PIN = 34;
const int BUTTON_PIN = 12;
int lastPotValue = -1;
bool lastButton = false;
uint8_t currentNote = 60; // Middle C

void setup() {
    Serial.begin(115200);
    MIDI.begin(1);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    Serial.println("MIDI Controller ready");
}

void loop() {
    // Read pot for CC (mod wheel)
    int potValue = analogRead(POT_PIN);
    int midiValue = map(potValue, 0, 4095, 0, 127);
    if (midiValue != lastPotValue) {
        MIDI.sendControlChange(1, midiValue, 1);
        lastPotValue = midiValue;
    }

    // Button triggers note
    bool button = (digitalRead(BUTTON_PIN) == LOW);
    if (button && !lastButton) {
        MIDI.sendNoteOn(currentNote, 127, 1);
        Serial.print("Note ON: ");
        Serial.println(currentNote);
    } else if (!button && lastButton) {
        MIDI.sendNoteOff(currentNote, 0, 1);
        Serial.print("Note OFF: ");
        Serial.println(currentNote);
    }
    lastButton = button;

    // Process incoming MIDI
    if (MIDI.read()) {
        Serial.print("MIDI In: type=");
        Serial.print(MIDI.getType(), HEX);
        Serial.print(" d1=");
        Serial.print(MIDI.getData1());
        Serial.print(" d2=");
        Serial.println(MIDI.getData2());
    }

    delay(10);
}
