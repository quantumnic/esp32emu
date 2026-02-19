// Example: NVS persistent counter — stores boot count across reboots
#include <Arduino.h>
#include "nvs_flash.h"
#include <cstdio>

void setup() {
    nvs_flash_init();

    nvs_handle_t h;
    nvs_open("storage", NVS_READWRITE, &h);

    int32_t boot_count = 0;
    if (nvs_get_i32(h, "boot_count", &boot_count) == ESP_ERR_NVS_NOT_FOUND) {
        boot_count = 0;
    }
    boot_count++;
    nvs_set_i32(h, "boot_count", boot_count);
    nvs_commit(h);

    printf("Boot count: %d\n", boot_count);
    nvs_close(h);
}

void loop() {
    delay(1000);
}
