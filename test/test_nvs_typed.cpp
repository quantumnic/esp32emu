#include "nvs_flash.h"
#include <cassert>
#include <cstdio>

int main() {
    esp32emu::nvs::NVSStore::instance().clear();
    assert(nvs_flash_init() == ESP_OK);

    nvs_handle_t h;
    assert(nvs_open("test", NVS_READWRITE, &h) == ESP_OK);

    // i8
    assert(nvs_set_i8(h, "i8val", -42) == ESP_OK);
    int8_t i8v;
    assert(nvs_get_i8(h, "i8val", &i8v) == ESP_OK);
    assert(i8v == -42);

    // u8
    assert(nvs_set_u8(h, "u8val", 200) == ESP_OK);
    uint8_t u8v;
    assert(nvs_get_u8(h, "u8val", &u8v) == ESP_OK);
    assert(u8v == 200);

    // i16
    assert(nvs_set_i16(h, "i16val", -1234) == ESP_OK);
    int16_t i16v;
    assert(nvs_get_i16(h, "i16val", &i16v) == ESP_OK);
    assert(i16v == -1234);

    // u16
    assert(nvs_set_u16(h, "u16val", 50000) == ESP_OK);
    uint16_t u16v;
    assert(nvs_get_u16(h, "u16val", &u16v) == ESP_OK);
    assert(u16v == 50000);

    // i64
    assert(nvs_set_i64(h, "i64val", -9876543210LL) == ESP_OK);
    int64_t i64v;
    assert(nvs_get_i64(h, "i64val", &i64v) == ESP_OK);
    assert(i64v == -9876543210LL);

    // u64
    assert(nvs_set_u64(h, "u64val", 18446744073709551000ULL) == ESP_OK);
    uint64_t u64v;
    assert(nvs_get_u64(h, "u64val", &u64v) == ESP_OK);
    assert(u64v == 18446744073709551000ULL);

    // Not found
    assert(nvs_get_i8(h, "nope", &i8v) == ESP_ERR_NVS_NOT_FOUND);

    nvs_close(h);
    printf("test_nvs_typed: all assertions passed\n");
    return 0;
}
