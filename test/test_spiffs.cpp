// esp32emu test — SPIFFS/LittleFS mock
#include "Arduino.h"
#include "SPIFFS.h"
#include <cassert>
#include <cstdio>

int main() {
    // SPIFFS
    assert(SPIFFS.begin(true));

    {
        auto f = SPIFFS.open("/config.json", "w");
        assert(f);
        f.print("{\"ssid\":\"test\"}");
        f.close();
    }

    assert(SPIFFS.exists("/config.json"));

    {
        auto f = SPIFFS.open("/config.json", "r");
        assert(f);
        std::string s = f.readString();
        assert(s.find("ssid") != std::string::npos);
        f.close();
    }

    assert(SPIFFS.remove("/config.json"));
    assert(!SPIFFS.exists("/config.json"));
    SPIFFS.end();

    // LittleFS
    assert(LittleFS.begin());

    {
        auto f = LittleFS.open("/data.txt", "w");
        assert(f);
        f.println("line1");
        f.close();
    }

    // Append
    {
        auto f = LittleFS.open("/data.txt", "a");
        assert(f);
        f.println("line2");
        f.close();
    }

    {
        auto f = LittleFS.open("/data.txt", "r");
        assert(f);
        std::string s = f.readString();
        assert(s.find("line1") != std::string::npos);
        assert(s.find("line2") != std::string::npos);
        f.close();
    }

    LittleFS.remove("/data.txt");
    LittleFS.end();

    printf("test_spiffs: all assertions passed\n");
    return 0;
}
