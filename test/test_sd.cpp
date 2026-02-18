// esp32emu test — SD card library
#include "Arduino.h"
#include "SD.h"
#include <cassert>
#include <cstdio>
#include <filesystem>

int main() {
    // Begin
    assert(SD.begin(5));

    // Write a file
    {
        File f = SD.open("/test.txt", FILE_WRITE);
        assert(f);
        f.println("Hello SD");
        f.println(42);
        f.close();
    }

    // File should exist
    assert(SD.exists("/test.txt"));

    // Read it back
    {
        File f = SD.open("/test.txt", FILE_READ);
        assert(f);
        std::string content = f.readString();
        assert(content.find("Hello SD") != std::string::npos);
        assert(content.find("42") != std::string::npos);
        f.close();
    }

    // mkdir + nested write
    assert(SD.mkdir("/logs"));
    {
        File f = SD.open("/logs/data.csv", FILE_WRITE);
        assert(f);
        f.print("temp,humidity\n");
        f.print("22.5,60\n");
        f.close();
    }
    assert(SD.exists("/logs/data.csv"));

    // Remove
    assert(SD.remove("/test.txt"));
    assert(!SD.exists("/test.txt"));

    // Cleanup
    SD.remove("/logs/data.csv");
    SD.rmdir("/logs");
    SD.end();

    printf("test_sd: all assertions passed\n");
    return 0;
}
