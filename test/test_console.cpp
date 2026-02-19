// esp32emu test — esp_console
#include "esp_console.h"
#include <cassert>
#include <cstdio>

static int cmd_version(int argc, char** argv) {
    (void)argc; (void)argv;
    printf("v1.0.0\n");
    return 0;
}

static int cmd_add(int argc, char** argv) {
    if (argc != 3) return 1;
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    printf("%d\n", a + b);
    return 0;
}

int main() {
    esp32emu::console_reset();

    // Init
    esp_console_config_t cfg = ESP_CONSOLE_CONFIG_DEFAULT();
    assert(esp_console_init(&cfg) == ESP_OK);

    // Register commands
    esp_console_cmd_t ver_cmd = {};
    ver_cmd.command = "version";
    ver_cmd.help = "Print version";
    ver_cmd.func = cmd_version;
    assert(esp_console_cmd_register(&ver_cmd) == ESP_OK);

    esp_console_cmd_t add_cmd = {};
    add_cmd.command = "add";
    add_cmd.help = "Add two numbers";
    add_cmd.func = cmd_add;
    assert(esp_console_cmd_register(&add_cmd) == ESP_OK);

    // Register help
    assert(esp_console_register_help_command() == ESP_OK);
    assert(esp32emu::console_command_count() == 3);

    // Run commands
    int ret = -1;
    assert(esp_console_run("version", &ret) == ESP_OK);
    assert(ret == 0);

    assert(esp_console_run("add 3 7", &ret) == ESP_OK);
    assert(ret == 0);

    // Command not found
    assert(esp_console_run("unknown", &ret) == ESP_ERR_NOT_FOUND);

    // Bad args to add returns 1
    assert(esp_console_run("add 1", &ret) == ESP_OK);
    assert(ret == 1);

    // Deinit
    assert(esp_console_deinit() == ESP_OK);
    assert(esp32emu::console_command_count() == 0);

    printf("test_console: all assertions passed\n");
    return 0;
}
