#include "MIDI.h"
#include <cassert>
#include <cstdio>

MIDI_CREATE_DEFAULT_INSTANCE()

int main() {
    MIDI.begin(1);
    assert(MIDI._test_hasBegun());

    // Send notes
    MIDI.sendNoteOn(60, 127, 1);
    MIDI.sendNoteOff(60, 0, 1);
    MIDI.sendControlChange(7, 100, 1);  // Volume
    MIDI.sendProgramChange(42, 1);
    MIDI.sendPitchBend(0, 1);

    auto sent = MIDI._test_getSent();
    assert(sent.size() == 5);
    assert(sent[0].type == midi::NoteOn);
    assert(sent[0].data1 == 60);
    assert(sent[0].data2 == 127);
    assert(sent[1].type == midi::NoteOff);
    assert(sent[2].type == midi::ControlChange);
    assert(sent[2].data1 == 7);
    assert(sent[2].data2 == 100);
    assert(sent[3].type == midi::ProgramChange);
    assert(sent[3].data1 == 42);

    // Receive with callbacks
    int noteOnCount = 0;
    int ccCount = 0;
    MIDI.setHandleNoteOn([&](uint8_t ch, uint8_t note, uint8_t vel) {
        assert(ch == 1);
        assert(note == 72);
        assert(vel == 100);
        noteOnCount++;
    });
    MIDI.setHandleControlChange([&](uint8_t ch, uint8_t num, uint8_t val) {
        assert(ch == 2);
        assert(num == 1);   // Mod wheel
        assert(val == 64);
        ccCount++;
    });

    MIDI._test_injectMessage(midi::NoteOn, 1, 72, 100);
    MIDI._test_injectMessage(midi::ControlChange, 2, 1, 64);

    assert(MIDI.read());
    assert(MIDI.getType() == midi::NoteOn);
    assert(noteOnCount == 1);

    assert(MIDI.read());
    assert(MIDI.getType() == midi::ControlChange);
    assert(ccCount == 1);

    // No more messages
    assert(!MIDI.read());

    MIDI._test_clearSent();
    assert(MIDI._test_getSentCount() == 0);

    printf("test_midi: all assertions passed\n");
    return 0;
}
