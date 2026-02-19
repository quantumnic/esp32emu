// ESP-IDF Console CLI example
// Demonstrates command-line interface with registered commands
#include <Arduino.h>
#include "esp_console.h"

static int cmd_led(int argc, char** argv) {
    if (argc < 2) {
        Serial.println("Usage: led <on|off>");
        return 1;
    }
    if (strcmp(argv[1], "on") == 0) {
        digitalWrite(LED_BUILTIN, HIGH);
        Serial.println("LED ON");
    } else {
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println("LED OFF");
    }
    return 0;
}

static int cmd_gpio(int argc, char** argv) {
    if (argc < 2) {
        Serial.println("Usage: gpio <pin>");
        return 1;
    }
    int pin = atoi(argv[1]);
    Serial.print("GPIO ");
    Serial.print(pin);
    Serial.print(" = ");
    Serial.println(digitalRead(pin));
    return 0;
}

void setup() {
    Serial.begin(115200);
    pinMode(LED_BUILTIN, OUTPUT);

    esp_console_config_t cfg = ESP_CONSOLE_CONFIG_DEFAULT();
    esp_console_init(&cfg);

    esp_console_cmd_t led_cmd = {};
    led_cmd.command = "led";
    led_cmd.help = "Control built-in LED";
    led_cmd.func = cmd_led;
    esp_console_cmd_register(&led_cmd);

    esp_console_cmd_t gpio_cmd = {};
    gpio_cmd.command = "gpio";
    gpio_cmd.help = "Read GPIO pin state";
    gpio_cmd.func = cmd_gpio;
    esp_console_cmd_register(&gpio_cmd);

    esp_console_register_help_command();
    Serial.println("Console ready. Type 'help' for commands.");

    // Demo: run some commands
    int ret;
    esp_console_run("help", &ret);
    esp_console_run("led on", &ret);
    esp_console_run("gpio 2", &ret);
    esp_console_run("led off", &ret);
}

void loop() {
    delay(1000);
}
