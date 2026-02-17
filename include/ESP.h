#pragma once
#include <cstdint>
#include <cstdlib>
#include <cstdio>

class EspClass {
public:
    uint32_t getFreeHeap()     { return 256000; }
    uint32_t getHeapSize()     { return 327680; }
    uint32_t getMinFreeHeap()  { return 200000; }
    uint32_t getChipId()       { return 0xDEADBEEF; }
    uint32_t getCpuFreqMHz()   { return 240; }
    uint32_t getFlashChipSize(){ return 4194304; }
    const char* getSdkVersion(){ return "esp32emu-1.0.0"; }
    const char* getChipModel() { return "ESP32-EMU"; }
    uint8_t getChipRevision()  { return 3; }
    uint8_t getChipCores()     { return 2; }

    void restart() {
        fprintf(stderr, "[esp32emu] ESP.restart() called — exiting.\n");
        exit(0);
    }

    void deepSleep(uint64_t us) {
        fprintf(stderr, "[esp32emu] ESP.deepSleep(%llu µs) — exiting.\n",
                (unsigned long long)us);
        exit(0);
    }
};

extern EspClass ESP;
