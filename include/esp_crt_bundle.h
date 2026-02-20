#pragma once
// esp32emu — esp_crt_bundle stub
// Provides the certificate bundle attach function used in HTTPS examples.

#include <cstdio>

#ifndef ESP_OK
#define ESP_OK 0
#endif

typedef int esp_err_t;

inline esp_err_t esp_crt_bundle_attach(void* /*conf*/) {
    fprintf(stderr, "[esp32emu] CRT bundle: attached (stub)\n");
    return ESP_OK;
}
