#include "esp_wear_levelling.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    wl_handle_t handle = WL_INVALID_HANDLE;

    // Mount
    assert(wl_mount(nullptr, &handle) == ESP_OK);
    assert(handle != WL_INVALID_HANDLE);
    assert(wl_size(handle) == 4096 * 16);
    assert(wl_sector_size(handle) == 4096);

    // Write and read back
    const char msg[] = "Hello WearLevelling";
    assert(wl_write(handle, 0, msg, sizeof(msg)) == ESP_OK);

    char buf[64] = {};
    assert(wl_read(handle, 0, buf, sizeof(msg)) == ESP_OK);
    assert(strcmp(buf, msg) == 0);

    // Write at offset
    const char msg2[] = "Offset data";
    assert(wl_write(handle, 4096, msg2, sizeof(msg2)) == ESP_OK);
    char buf2[64] = {};
    assert(wl_read(handle, 4096, buf2, sizeof(msg2)) == ESP_OK);
    assert(strcmp(buf2, msg2) == 0);

    // Erase range (fills with 0xFF)
    assert(wl_erase_range(handle, 0, 4096) == ESP_OK);
    uint8_t byte;
    assert(wl_read(handle, 0, &byte, 1) == ESP_OK);
    assert(byte == 0xFF);

    // Data at offset 4096 still intact
    char buf3[64] = {};
    assert(wl_read(handle, 4096, buf3, sizeof(msg2)) == ESP_OK);
    assert(strcmp(buf3, msg2) == 0);

    // Out of bounds
    assert(wl_read(handle, 100000, buf, 1) == ESP_ERR_INVALID_ARG);
    assert(wl_write(handle, 100000, buf, 1) == ESP_ERR_INVALID_ARG);

    // Unmount
    assert(wl_unmount(handle) == ESP_OK);
    assert(wl_size(handle) == 0); // gone

    printf("test_wear_levelling: all assertions passed\n");
    return 0;
}
