#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"
#include <cassert>
#include <cstdio>

int main() {
    DFRobotDFPlayerMini mp3;
    HardwareSerial mySerial;
    assert(mp3.begin(mySerial));
    assert(mp3.test_isStarted());

    // Volume
    mp3.volume(20);
    assert(mp3.test_getVolume() == 20);
    assert(mp3.readVolume() == 20);
    mp3.volumeUp();
    assert(mp3.test_getVolume() == 21);
    mp3.volumeDown();
    mp3.volumeDown();
    assert(mp3.test_getVolume() == 19);

    // Volume clamp
    mp3.volume(50);
    assert(mp3.test_getVolume() == 30);

    // Play
    mp3.play(3);
    assert(mp3.test_isPlaying());
    assert(mp3.test_getCurrentTrack() == 3);
    assert(mp3.readCurrentFileNumber() == 3);

    // Pause/resume
    mp3.pause();
    assert(mp3.test_isPaused());
    assert(!mp3.test_isPlaying());
    mp3.start();
    assert(mp3.test_isPlaying());
    assert(!mp3.test_isPaused());

    // Stop
    mp3.stop();
    assert(!mp3.test_isPlaying());

    // Play folder
    mp3.playFolder(2, 5);
    assert(mp3.test_getCurrentFolder() == 2);
    assert(mp3.test_getCurrentTrack() == 5);

    // Next/previous
    mp3.next();
    assert(mp3.test_getCurrentTrack() == 6);
    mp3.previous();
    assert(mp3.test_getCurrentTrack() == 5);

    // Loop
    mp3.loop(1);
    assert(mp3.test_isLooping());
    assert(mp3.test_isPlaying());
    mp3.disableLoop();
    assert(!mp3.test_isLooping());

    // EQ
    mp3.EQ(3);
    assert(mp3.test_getEQ() == 3);
    assert(mp3.readEQ() == 3);

    // File counts
    mp3.test_setTotalFiles(25);
    assert(mp3.readFileCounts() == 25);

    // Sleep/wake
    mp3.sleep();
    assert(mp3.test_isSleeping());
    assert(!mp3.test_isPlaying());
    mp3.reset();
    assert(!mp3.test_isSleeping());

    // DAC
    mp3.disableDAC();
    assert(!mp3.test_isDACEnabled());
    mp3.enableDAC();
    assert(mp3.test_isDACEnabled());

    // Message availability
    mp3.test_setAvailable(true, DFRobotDFPlayerMini::DFPlayerPlayFinished, 3);
    assert(mp3.available());
    assert(mp3.readType() == DFRobotDFPlayerMini::DFPlayerPlayFinished);
    assert(mp3.read() == 3);

    printf("test_dfplayer: all assertions passed\n");
    return 0;
}
