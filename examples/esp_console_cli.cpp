// esp32emu example — ESP-IDF Console CLI
// Register custom commands and run them.
#include "Arduino.h"
#include "esp_console.h"

static int cmd_status(int argc, char** argv) {
    (void)argc; (void)argv;
    Serial.println("System OK — uptime: " + String(millis() / 1000) + "s");
    return 0;
}

static int cmd_gpio(int argc, char** argv) {
    if (argc < 3) {
        Serial.println("Usage: gpio <pin> <0|1>");
        return 1;
    }
    int pin = atoi(argv[1]);
    int val = atoi(argv[2]);
    digitalWrite(pin, val);
    Serial.printf("GPIO %d → %d\n", pin, val);
    return 0;
}

void setup() {
    Serial.begin(115200);

    esp_console_config_t cfg = ESP_CONSOLE_CONFIG_DEFAULT();
    esp_console_init(&cfg);
    esp_console_register_help_command();

    esp_console_cmd_t status_cmd = {};
    status_cmd.command = "status";
    status_cmd.help = "Show system status";
    status_cmd.func = cmd_status;
    esp_console_cmd_register(&status_cmd);

    esp_console_cmd_t gpio_cmd = {};
    gpio_cmd.command = "gpio";
    gpio_cmd.help = "Set GPIO pin value";
    gpio_cmd.func = cmd_gpio;
    esp_console_cmd_register(&gpio_cmd);

    Serial.println("Console ready. Running commands:");

    int ret;
    esp_console_run("help", &ret);
    esp_console_run("status", &ret);
    esp_console_run("gpio 2 1", &ret);
}

void loop() { delay(1000); }
