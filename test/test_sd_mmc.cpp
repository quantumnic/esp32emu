// esp32emu — SD_MMC test
#include "SD_MMC.h"
#include <cassert>
#include <cstring>
#include <cstdio>

int main() {
    // Mount
    assert(SD_MMC.begin());

    // Write a file
    {
        auto f = SD_MMC.open("/test.txt", "w");
        assert(f);
        f.println("Hello SD_MMC");
        f.print("Line 2");
        f.close();
    }

    // Read it back
    {
        auto f = SD_MMC.open("/test.txt", "r");
        assert(f);
        assert(f.size() > 0);
        char buf[64] = {};
        size_t n = f.read((uint8_t*)buf, sizeof(buf) - 1);
        assert(n > 0);
        assert(strstr(buf, "Hello SD_MMC") != nullptr);
        assert(strstr(buf, "Line 2") != nullptr);
        f.close();
    }

    // Append
    {
        auto f = SD_MMC.open("/test.txt", "a");
        assert(f);
        f.println("\nAppended");
        f.close();
    }
    {
        auto f = SD_MMC.open("/test.txt", "r");
        char buf[128] = {};
        f.read((uint8_t*)buf, sizeof(buf) - 1);
        assert(strstr(buf, "Appended") != nullptr);
        f.close();
    }

    // exists / remove
    assert(SD_MMC.exists("/test.txt"));
    assert(SD_MMC.remove("/test.txt"));
    assert(!SD_MMC.exists("/test.txt"));

    // mkdir
    assert(SD_MMC.mkdir("/data"));
    assert(SD_MMC.exists("/data"));

    // rename
    {
        auto f = SD_MMC.open("/a.txt", "w");
        f.print("content");
        f.close();
    }
    assert(SD_MMC.rename("/a.txt", "/b.txt"));
    assert(!SD_MMC.exists("/a.txt"));
    assert(SD_MMC.exists("/b.txt"));

    // totalBytes / usedBytes
    assert(SD_MMC.totalBytes() > 0);
    assert(SD_MMC.usedBytes() > 0);

    // cardType
    assert(SD_MMC.cardType() == 2);

    // Directory listing
    {
        SD_MMC.mkdir("/mydir");
        auto f1 = SD_MMC.open("/mydir/one.txt", "w");
        f1.print("1"); f1.close();
        auto f2 = SD_MMC.open("/mydir/two.txt", "w");
        f2.print("2"); f2.close();

        auto dir = SD_MMC.open("/mydir", "r");
        assert(dir);
        assert(dir.isDirectory());
        int count = 0;
        while (true) {
            auto entry = dir.openNextFile();
            if (!entry) break;
            count++;
        }
        assert(count == 2);
    }

    // Unmount
    SD_MMC.end();

    printf("test_sd_mmc: all assertions passed\n");
    return 0;
}
