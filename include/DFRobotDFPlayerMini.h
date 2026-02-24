#pragma once
#include <cstdint>
#include <cstdio>

// DFPlayer Mini MP3 player mock
class DFRobotDFPlayerMini {
public:
    DFRobotDFPlayerMini() = default;

    bool begin(HardwareSerial& /*serial*/, bool isACK = true, bool doReset = true) {
        (void)isACK; (void)doReset;
        started_ = true;
        return true;
    }

    // Playback
    void play(int fileNumber) { current_track_ = fileNumber; playing_ = true; paused_ = false; }
    void playFolder(uint8_t folder, uint8_t fileNumber) {
        current_folder_ = folder; current_track_ = fileNumber; playing_ = true; paused_ = false;
    }
    void playLargeFolder(uint8_t folder, uint16_t fileNumber) {
        current_folder_ = folder; current_track_ = fileNumber; playing_ = true; paused_ = false;
    }
    void playMp3Folder(int fileNumber) { current_track_ = fileNumber; playing_ = true; paused_ = false; }
    void playAdvertisement(int fileNumber) { advert_track_ = fileNumber; }
    void stopAdvertisement() { advert_track_ = 0; }

    void next() { if (current_track_ < total_files_) current_track_++; }
    void previous() { if (current_track_ > 1) current_track_--; }

    void pause() { paused_ = true; playing_ = false; }
    void start() { paused_ = false; playing_ = true; }
    void stop() { playing_ = false; paused_ = false; }

    // Volume
    void volume(uint8_t vol) { volume_ = vol > 30 ? 30 : vol; }
    void volumeUp() { if (volume_ < 30) volume_++; }
    void volumeDown() { if (volume_ > 0) volume_--; }

    // EQ
    void EQ(uint8_t eq) { eq_ = eq; }

    // Loop
    void loop(int fileNumber) { current_track_ = fileNumber; looping_ = true; playing_ = true; }
    void enableLoop() { looping_ = true; }
    void disableLoop() { looping_ = false; }
    void enableLoopAll() { loop_all_ = true; }
    void disableLoopAll() { loop_all_ = false; }

    // Output device
    void outputDevice(uint8_t device) { output_device_ = device; }

    // Sleep/wake
    void sleep() { sleeping_ = true; playing_ = false; }
    void reset() { sleeping_ = false; playing_ = false; current_track_ = 0; volume_ = 15; }

    // Status
    int readState() { return playing_ ? 1 : 0; }
    int readVolume() { return volume_; }
    int readEQ() { return eq_; }
    int readFileCounts() { return total_files_; }
    int readCurrentFileNumber() { return current_track_; }
    int readFileCountsInFolder(int folder) { (void)folder; return folder_file_count_; }

    bool available() { return available_; }
    uint8_t readType() { return msg_type_; }
    int read() { return msg_value_; }

    // DAC
    void enableDAC() { dac_enabled_ = true; }
    void disableDAC() { dac_enabled_ = false; }

    // Test helpers
    void test_setTotalFiles(int n) { total_files_ = n; }
    void test_setFolderFileCount(int n) { folder_file_count_ = n; }
    void test_setAvailable(bool a, uint8_t type = 0, int value = 0) {
        available_ = a; msg_type_ = type; msg_value_ = value;
    }
    bool test_isStarted() const { return started_; }
    bool test_isPlaying() const { return playing_; }
    bool test_isPaused() const { return paused_; }
    bool test_isLooping() const { return looping_; }
    bool test_isSleeping() const { return sleeping_; }
    int test_getCurrentTrack() const { return current_track_; }
    uint8_t test_getCurrentFolder() const { return current_folder_; }
    uint8_t test_getVolume() const { return volume_; }
    uint8_t test_getEQ() const { return eq_; }
    bool test_isDACEnabled() const { return dac_enabled_; }

    // DFPlayer message types
    static constexpr uint8_t DFPlayerPlayFinished = 0x3D;
    static constexpr uint8_t DFPlayerError = 0x40;
    static constexpr uint8_t DFPlayerCardInserted = 0x3A;
    static constexpr uint8_t DFPlayerCardRemoved = 0x3B;
    static constexpr uint8_t DFPlayerUSBInserted = 0x3C;

private:
    bool started_ = false;
    bool playing_ = false;
    bool paused_ = false;
    bool looping_ = false;
    bool loop_all_ = false;
    bool sleeping_ = false;
    bool dac_enabled_ = true;
    bool available_ = false;
    uint8_t volume_ = 15;
    uint8_t eq_ = 0;
    uint8_t output_device_ = 1;
    int current_track_ = 0;
    uint8_t current_folder_ = 0;
    int advert_track_ = 0;
    int total_files_ = 10;
    int folder_file_count_ = 5;
    uint8_t msg_type_ = 0;
    int msg_value_ = 0;
};
