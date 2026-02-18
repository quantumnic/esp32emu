#include "Preferences.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    Preferences prefs;

    // Basic int storage
    assert(prefs.begin("test_ns", false));
    prefs.putInt("counter", 42);
    assert(prefs.getInt("counter") == 42);
    assert(prefs.getInt("missing", -1) == -1);

    // String storage
    prefs.putString("name", "esp32emu");
    assert(prefs.getString("name") == "esp32emu");
    assert(prefs.getString("missing", "default") == "default");

    // Bool
    prefs.putBool("flag", true);
    assert(prefs.getBool("flag") == true);
    assert(prefs.getBool("nope", false) == false);

    // Float
    prefs.putFloat("pi", 3.14f);
    float pi = prefs.getFloat("pi");
    assert(pi > 3.13f && pi < 3.15f);

    // Bytes
    uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF};
    prefs.putBytes("blob", data, 4);
    uint8_t out[4] = {};
    assert(prefs.getBytes("blob", out, 4) == 4);
    assert(memcmp(data, out, 4) == 0);

    // isKey
    assert(prefs.isKey("counter"));
    assert(!prefs.isKey("nonexistent"));

    // remove
    assert(prefs.remove("counter"));
    assert(!prefs.isKey("counter"));
    assert(prefs.getInt("counter", -1) == -1);

    // clear
    prefs.putInt("a", 1);
    prefs.putInt("b", 2);
    prefs.clear();
    assert(!prefs.isKey("a"));
    assert(!prefs.isKey("b"));

    // readOnly mode
    prefs.end();
    prefs.begin("ro_ns", false);
    prefs.putInt("val", 100);
    prefs.end();

    prefs.begin("ro_ns", true);
    assert(prefs.getInt("val") == 100);
    assert(prefs.putInt("val", 200) == 0); // should fail
    assert(prefs.getInt("val") == 100);    // unchanged
    prefs.end();

    // Namespace isolation
    Preferences prefs2;
    prefs.begin("ns1", false);
    prefs2.begin("ns2", false);
    prefs.putInt("x", 10);
    prefs2.putInt("x", 20);
    assert(prefs.getInt("x") == 10);
    assert(prefs2.getInt("x") == 20);
    prefs.end();
    prefs2.end();

    printf("test_preferences: all assertions passed\n");
    return 0;
}
