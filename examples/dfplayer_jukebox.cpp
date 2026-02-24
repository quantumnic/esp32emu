// DFPlayer Mini MP3 jukebox example
// Simple serial-controlled music player

#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>

DFRobotDFPlayerMini mp3;
HardwareSerial mySerial(2);

void setup() {
    Serial.begin(115200);
    mySerial.begin(9600);

    if (!mp3.begin(mySerial)) {
        Serial.println("DFPlayer Mini not found!");
        while (1) delay(10);
    }

    Serial.println("🎵 DFPlayer Mini Jukebox");
    Serial.println("========================");

    mp3.volume(20);
    mp3.EQ(0);  // Normal EQ

    mp3.test_setTotalFiles(15);

    Serial.println("Commands: p=play, s=stop, +=vol up, -=vol down, n=next, b=prev");
    Serial.print("Total tracks: ");
    Serial.println(mp3.readFileCounts());

    // Auto-play track 1
    mp3.play(1);
    Serial.println("▶️  Now playing: Track 1");
}

void printStatus() {
    Serial.print("  Track: ");
    Serial.print(mp3.readCurrentFileNumber());
    Serial.print("  Volume: ");
    Serial.print(mp3.readVolume());
    Serial.print("/30  State: ");
    Serial.println(mp3.readState() ? "Playing" : "Stopped");
}

void loop() {
    // Check for finished playback
    if (mp3.available()) {
        uint8_t type = mp3.readType();
        int value = mp3.read();

        if (type == DFRobotDFPlayerMini::DFPlayerPlayFinished) {
            Serial.print("⏹️  Track ");
            Serial.print(value);
            Serial.println(" finished");
            mp3.next();
            Serial.println("▶️  Playing next track");
        }
    }

    // Simulate user input
    static int tick = 0;
    tick++;

    if (tick == 3) {
        mp3.volumeUp();
        Serial.println("🔊 Volume up");
        printStatus();
    } else if (tick == 5) {
        mp3.next();
        Serial.println("⏭️  Next track");
        printStatus();
    } else if (tick == 7) {
        mp3.playFolder(2, 3);
        Serial.println("📂 Playing folder 2, track 3");
        printStatus();
    } else if (tick == 9) {
        mp3.pause();
        Serial.println("⏸️  Paused");
        printStatus();
    } else if (tick == 10) {
        mp3.start();
        Serial.println("▶️  Resumed");
        printStatus();
    }

    delay(1000);
}
