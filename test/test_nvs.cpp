// esp32emu — test_nvs: NVS Flash mock test
#include "nvs_flash.h"
#include <cassert>
#include <cstdio>
#include <cstring>

int main() {
    // Init
    assert(nvs_flash_init() == ESP_OK);

    // Open namespace
    nvs_handle_t h;
    assert(nvs_open("test_ns", NVS_READWRITE, &h) == ESP_OK);

    // i32 set/get
    assert(nvs_set_i32(h, "counter", 42) == ESP_OK);
    assert(nvs_commit(h) == ESP_OK);
    int32_t val;
    assert(nvs_get_i32(h, "counter", &val) == ESP_OK);
    assert(val == 42);

    // u32 set/get
    assert(nvs_set_u32(h, "flags", 0xDEADBEEF) == ESP_OK);
    uint32_t uval;
    assert(nvs_get_u32(h, "flags", &uval) == ESP_OK);
    assert(uval == 0xDEADBEEF);

    // String set/get
    assert(nvs_set_str(h, "name", "esp32emu") == ESP_OK);
    size_t len = 0;
    assert(nvs_get_str(h, "name", nullptr, &len) == ESP_OK);
    assert(len == 9); // "esp32emu" + null
    char buf[32];
    len = sizeof(buf);
    assert(nvs_get_str(h, "name", buf, &len) == ESP_OK);
    assert(strcmp(buf, "esp32emu") == 0);

    // Blob set/get
    uint8_t blob_out[4] = {1, 2, 3, 4};
    assert(nvs_set_blob(h, "data", blob_out, 4) == ESP_OK);
    uint8_t blob_in[4] = {};
    size_t blen = 4;
    assert(nvs_get_blob(h, "data", blob_in, &blen) == ESP_OK);
    assert(blen == 4);
    assert(memcmp(blob_out, blob_in, 4) == 0);

    // Not found
    int32_t missing;
    assert(nvs_get_i32(h, "nope", &missing) == ESP_ERR_NVS_NOT_FOUND);

    // Erase key
    assert(nvs_erase_key(h, "counter") == ESP_OK);
    assert(nvs_get_i32(h, "counter", &val) == ESP_ERR_NVS_NOT_FOUND);

    // Erase all
    assert(nvs_erase_all(h) == ESP_OK);
    assert(nvs_get_str(h, "name", nullptr, &len) == ESP_ERR_NVS_NOT_FOUND);

    // Close
    nvs_close(h);

    // Read-only handle can't write
    nvs_handle_t ro;
    assert(nvs_open("test_ns", NVS_READONLY, &ro) == ESP_OK);
    assert(nvs_set_i32(ro, "x", 1) == ESP_FAIL);
    nvs_close(ro);

    // Flash erase
    assert(nvs_flash_erase() == ESP_OK);

    printf("test_nvs: all assertions passed\n");
    return 0;
}
