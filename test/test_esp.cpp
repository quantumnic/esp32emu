#include "ESP.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    assert(ESP.getFreeHeap() > 0);
    assert(ESP.getHeapSize() > ESP.getFreeHeap());
    assert(ESP.getChipId() == 0xDEADBEEF);
    assert(ESP.getCpuFreqMHz() == 240);
    assert(ESP.getChipCores() == 2);
    assert(strcmp(ESP.getChipModel(), "ESP32-EMU") == 0);
    assert(ESP.getFlashChipSize() == 4194304);
    assert(strlen(ESP.getSdkVersion()) > 0);

    printf("test_esp: all assertions passed\n");
    return 0;
}
