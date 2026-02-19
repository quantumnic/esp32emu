#pragma once
// esp32emu — ESP-IDF console component mock
// Provides esp_console command registration & dispatch for host-side testing.

#include "esp_err.h"
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <functional>
#include <sstream>

namespace esp32emu {

struct ConsoleCommand {
    std::string command;
    std::string help;
    std::function<int(int argc, char** argv)> func;
    void* hint = nullptr;
};

inline std::vector<ConsoleCommand>& console_commands() {
    static std::vector<ConsoleCommand> cmds;
    return cmds;
}

} // namespace esp32emu

// ESP-IDF console types
struct esp_console_cmd_t {
    const char* command;
    const char* help;
    const char* hint;
    int (*func)(int argc, char** argv);
    void* argtable;
};

struct esp_console_config_t {
    uint32_t max_cmdline_length;
    uint32_t max_cmdline_args;
    int hint_color;
    int hint_bold;
};

#define ESP_CONSOLE_CONFIG_DEFAULT() \
    { .max_cmdline_length = 256, .max_cmdline_args = 32, .hint_color = 39, .hint_bold = 0 }

inline esp_err_t esp_console_init(const esp_console_config_t* config) {
    (void)config;
    esp32emu::console_commands().clear();
    return ESP_OK;
}

inline esp_err_t esp_console_deinit(void) {
    esp32emu::console_commands().clear();
    return ESP_OK;
}

inline esp_err_t esp_console_cmd_register(const esp_console_cmd_t* cmd) {
    if (!cmd || !cmd->command || !cmd->func) return ESP_ERR_INVALID_ARG;
    esp32emu::ConsoleCommand c;
    c.command = cmd->command;
    c.help = cmd->help ? cmd->help : "";
    c.func = cmd->func;
    esp32emu::console_commands().push_back(c);
    return ESP_OK;
}

// Parse a command line string into argc/argv and dispatch
inline esp_err_t esp_console_run(const char* cmdline, int* ret_val) {
    if (!cmdline || !ret_val) return ESP_ERR_INVALID_ARG;

    // Tokenize
    std::vector<std::string> tokens;
    std::istringstream iss(cmdline);
    std::string tok;
    while (iss >> tok) tokens.push_back(tok);

    if (tokens.empty()) {
        *ret_val = 0;
        return ESP_ERR_INVALID_ARG;
    }

    // Find command
    for (auto& c : esp32emu::console_commands()) {
        if (c.command == tokens[0]) {
            std::vector<char*> argv;
            for (auto& t : tokens) argv.push_back(const_cast<char*>(t.c_str()));
            *ret_val = c.func((int)argv.size(), argv.data());
            return ESP_OK;
        }
    }
    *ret_val = -1;
    return ESP_ERR_NOT_FOUND;
}

// Register built-in 'help' command
inline esp_err_t esp_console_register_help_command(void) {
    esp_console_cmd_t help_cmd = {};
    help_cmd.command = "help";
    help_cmd.help = "Print list of registered commands";
    help_cmd.func = [](int, char**) -> int {
        for (auto& c : esp32emu::console_commands()) {
            printf("  %-16s %s\n", c.command.c_str(), c.help.c_str());
        }
        return 0;
    };
    return esp_console_cmd_register(&help_cmd);
}

// Test helpers
namespace esp32emu {
inline size_t console_command_count() { return console_commands().size(); }
inline void console_reset() { console_commands().clear(); }
} // namespace esp32emu
