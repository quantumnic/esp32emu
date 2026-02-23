#pragma once
// esp32emu mock: Arduino MIDI Library
#include <cstdint>
#include <vector>
#include <functional>

namespace midi {

enum MidiType : uint8_t {
    NoteOff             = 0x80,
    NoteOn              = 0x90,
    AfterTouchPoly      = 0xA0,
    ControlChange       = 0xB0,
    ProgramChange       = 0xC0,
    AfterTouchChannel   = 0xD0,
    PitchBend           = 0xE0,
    SystemExclusive     = 0xF0,
    Clock               = 0xF8,
    Start               = 0xFA,
    Continue            = 0xFB,
    Stop                = 0xFC,
    ActiveSensing       = 0xFE,
    SystemReset         = 0xFF,
};

struct MidiMessage {
    MidiType type;
    uint8_t channel;
    uint8_t data1;
    uint8_t data2;
};

enum Channel : uint8_t {
    CHANNEL_OMNI = 0,
};

} // namespace midi

template <class Transport>
class MidiInterface {
public:
    using NoteOnCallback    = std::function<void(uint8_t channel, uint8_t note, uint8_t velocity)>;
    using NoteOffCallback   = std::function<void(uint8_t channel, uint8_t note, uint8_t velocity)>;
    using CCCallback        = std::function<void(uint8_t channel, uint8_t number, uint8_t value)>;
    using PCCallback        = std::function<void(uint8_t channel, uint8_t number)>;
    using PitchBendCallback = std::function<void(uint8_t channel, int bend)>;

    MidiInterface() = default;

    void begin(uint8_t channel = 1) { _channel = channel; _begun = true; }

    void sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
        _sent.push_back({midi::NoteOn, channel, note, velocity});
    }
    void sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
        _sent.push_back({midi::NoteOff, channel, note, velocity});
    }
    void sendControlChange(uint8_t number, uint8_t value, uint8_t channel) {
        _sent.push_back({midi::ControlChange, channel, number, value});
    }
    void sendProgramChange(uint8_t number, uint8_t channel) {
        _sent.push_back({midi::ProgramChange, channel, number, 0});
    }
    void sendPitchBend(int value, uint8_t channel) {
        _sent.push_back({midi::PitchBend, channel, (uint8_t)(value & 0x7F), (uint8_t)((value >> 7) & 0x7F)});
    }

    bool read() {
        if (_rxQueue.empty()) return false;
        _lastMsg = _rxQueue.front();
        _rxQueue.erase(_rxQueue.begin());
        // Dispatch callbacks
        switch (_lastMsg.type) {
            case midi::NoteOn:
                if (_onNoteOn) _onNoteOn(_lastMsg.channel, _lastMsg.data1, _lastMsg.data2);
                break;
            case midi::NoteOff:
                if (_onNoteOff) _onNoteOff(_lastMsg.channel, _lastMsg.data1, _lastMsg.data2);
                break;
            case midi::ControlChange:
                if (_onCC) _onCC(_lastMsg.channel, _lastMsg.data1, _lastMsg.data2);
                break;
            case midi::ProgramChange:
                if (_onPC) _onPC(_lastMsg.channel, _lastMsg.data1);
                break;
            default: break;
        }
        return true;
    }

    midi::MidiType getType() const { return _lastMsg.type; }
    uint8_t getChannel() const { return _lastMsg.channel; }
    uint8_t getData1() const { return _lastMsg.data1; }
    uint8_t getData2() const { return _lastMsg.data2; }

    void setHandleNoteOn(NoteOnCallback cb) { _onNoteOn = cb; }
    void setHandleNoteOff(NoteOffCallback cb) { _onNoteOff = cb; }
    void setHandleControlChange(CCCallback cb) { _onCC = cb; }
    void setHandleProgramChange(PCCallback cb) { _onPC = cb; }
    void setHandlePitchBend(PitchBendCallback cb) { _onPitchBend = cb; }

    // ── Test helpers ──
    void _test_injectMessage(midi::MidiType type, uint8_t channel, uint8_t d1, uint8_t d2 = 0) {
        _rxQueue.push_back({type, channel, d1, d2});
    }
    std::vector<midi::MidiMessage> _test_getSent() const { return _sent; }
    void _test_clearSent() { _sent.clear(); }
    size_t _test_getSentCount() const { return _sent.size(); }
    bool _test_hasBegun() const { return _begun; }

private:
    uint8_t _channel = 1;
    bool _begun = false;
    midi::MidiMessage _lastMsg{};
    std::vector<midi::MidiMessage> _rxQueue;
    std::vector<midi::MidiMessage> _sent;
    NoteOnCallback _onNoteOn;
    NoteOffCallback _onNoteOff;
    CCCallback _onCC;
    PCCallback _onPC;
    PitchBendCallback _onPitchBend;
};

// Default serial transport (mock)
struct SerialMIDI {
    SerialMIDI() = default;
};

namespace MIDI_NAMESPACE = midi;

// Convenience: create a default MIDI instance
static inline MidiInterface<SerialMIDI>& MIDI_getInstance() {
    static MidiInterface<SerialMIDI> instance;
    return instance;
}

#define MIDI_CREATE_DEFAULT_INSTANCE() \
    static MidiInterface<SerialMIDI> MIDI;

#define MIDI_CREATE_INSTANCE(Type, Port, Name) \
    static MidiInterface<SerialMIDI> Name;
