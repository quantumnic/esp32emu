// esp32emu — esp_console test
#include "Arduino.h"
#include "esp_console.h"
#include <cassert>
#include <cstdio>

static int cmd_hello(int argc, char** argv) {
    (void)argc; (void)argv;
    return 0;
}

static int cmd_add(int argc, char** argv) {
    if (argc < 3) return 1;
    return atoi(argv[1]) + atoi(argv[2]);
}

int main() {
    // Init
    esp_console_config_t cfg = ESP_CONSOLE_CONFIG_DEFAULT();
    assert(esp_console_init(&cfg) == ESP_OK);
    assert(esp32emu::console_command_count() == 0);

    // Register commands
    esp_console_cmd_t hello = {};
    hello.command = "hello";
    hello.help = "Say hello";
    hello.func = cmd_hello;
    assert(esp_console_cmd_register(&hello) == ESP_OK);
    assert(esp32emu::console_command_count() == 1);

    esp_console_cmd_t add = {};
    add.command = "add";
    add.help = "Add two numbers";
    add.func = cmd_add;
    assert(esp_console_cmd_register(&add) == ESP_OK);
    assert(esp32emu::console_command_count() == 2);

    // Run commands
    int ret = -1;
    assert(esp_console_run("hello", &ret) == ESP_OK);
    assert(ret == 0);

    assert(esp_console_run("add 3 4", &ret) == ESP_OK);
    assert(ret == 7);

    // Not found
    assert(esp_console_run("unknown", &ret) == ESP_ERR_NOT_FOUND);

    // Invalid args
    assert(esp_console_run(nullptr, &ret) == ESP_ERR_INVALID_ARG);

    // Register help
    assert(esp_console_register_help_command() == ESP_OK);
    assert(esp32emu::console_command_count() == 3);
    assert(esp_console_run("help", &ret) == ESP_OK);

    // Deinit
    assert(esp_console_deinit() == ESP_OK);
    assert(esp32emu::console_command_count() == 0);

    printf("test_esp_console: all assertions passed\n");
    return 0;
}
