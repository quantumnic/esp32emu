#pragma once
#include <cstdint>
#include <cstdio>

class Adafruit_DRV2605 {
public:
    Adafruit_DRV2605() {}

    bool begin() { _started = true; return true; }

    void setWaveform(uint8_t slot, uint8_t waveform) {
        if (slot < 8) _waveforms[slot] = waveform;
    }
    void go() { _playing = true; }
    void stop() { _playing = false; }

    void setMode(uint8_t mode) { _mode = mode; }
    uint8_t getMode() const { return _mode; }

    void setRealtimeValue(uint8_t val) { _rtp_value = val; }

    void selectLibrary(uint8_t lib) { _library = lib; }
    uint8_t readLibrary() const { return _library; }

    void useERM() { _erm = true; }
    void useLRA() { _erm = false; }
    bool isERM() const { return _erm; }

    // Modes
    static constexpr uint8_t DRV2605_MODE_INTTRIG   = 0x00;
    static constexpr uint8_t DRV2605_MODE_EXTTRIGEDGE = 0x01;
    static constexpr uint8_t DRV2605_MODE_EXTTRIGLVL  = 0x02;
    static constexpr uint8_t DRV2605_MODE_PWMANALOG  = 0x03;
    static constexpr uint8_t DRV2605_MODE_AUDIOVIBE  = 0x04;
    static constexpr uint8_t DRV2605_MODE_REALTIME   = 0x05;
    static constexpr uint8_t DRV2605_MODE_DIAGNOS    = 0x06;
    static constexpr uint8_t DRV2605_MODE_AUTOCAL     = 0x07;

    // Test helpers
    bool _test_is_playing() const { return _playing; }
    uint8_t _test_get_waveform(uint8_t slot) const { return slot < 8 ? _waveforms[slot] : 0; }
    uint8_t _test_get_rtp_value() const { return _rtp_value; }

private:
    bool _started = false;
    bool _playing = false;
    bool _erm = true;
    uint8_t _mode = 0;
    uint8_t _library = 1;
    uint8_t _rtp_value = 0;
    uint8_t _waveforms[8] = {};
};
