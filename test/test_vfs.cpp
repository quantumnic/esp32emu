#include "esp_vfs.h"
#include <cassert>
#include <cstdio>

static int dummy_open(const char*, int, int) { return 1; }
static int dummy_close(int) { return 0; }

int main() {
    esp32emu::vfs::reset();
    assert(esp32emu::vfs::count() == 0);

    // Register
    esp_vfs_t vfs = {};
    vfs.open = dummy_open;
    vfs.close = dummy_close;

    assert(esp_vfs_register("/data", &vfs, nullptr) == ESP_OK);
    assert(esp32emu::vfs::isRegistered("/data"));
    assert(esp32emu::vfs::count() == 1);

    // Register another
    assert(esp_vfs_register("/sdcard", &vfs, nullptr) == ESP_OK);
    assert(esp32emu::vfs::count() == 2);

    // Invalid args
    assert(esp_vfs_register(nullptr, &vfs, nullptr) == ESP_ERR_INVALID_ARG);
    assert(esp_vfs_register("/x", nullptr, nullptr) == ESP_ERR_INVALID_ARG);

    // Unregister
    assert(esp_vfs_unregister("/data") == ESP_OK);
    assert(!esp32emu::vfs::isRegistered("/data"));
    assert(esp32emu::vfs::count() == 1);

    // Unregister non-existent
    assert(esp_vfs_unregister("/nope") == ESP_ERR_NOT_FOUND);

    // Cleanup
    esp32emu::vfs::reset();
    assert(esp32emu::vfs::count() == 0);

    printf("test_vfs: all assertions passed\n");
    return 0;
}
