// Example: ESP-IDF GPIO driver with interrupts
// Demonstrates driver/gpio.h mock
#include <Arduino.h>
#include "driver/gpio.h"
#include <cstdio>
#include <atomic>

static std::atomic<int> buttonPresses{0};

static void button_isr(void* arg) {
    (void)arg;
    buttonPresses++;
}

void setup() {
    // Configure button on GPIO4 with interrupt
    gpio_set_direction(GPIO_NUM_4, GPIO_MODE_INPUT);
    gpio_set_pull_mode(GPIO_NUM_4, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(GPIO_NUM_4, GPIO_INTR_NEGEDGE);

    gpio_install_isr_service(0);
    gpio_isr_handler_add(GPIO_NUM_4, button_isr, nullptr);

    // Configure LED on GPIO2
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_2, 0);

    printf("[gpio_interrupt] Button on GPIO4, LED on GPIO2\n");

    // Simulate 3 button presses
    auto& drv = esp32emu::GpioDriver::instance();
    drv.setLevel(GPIO_NUM_4, 1); // initial high (pulled up)
    for (int i = 0; i < 3; i++) {
        drv.setLevel(GPIO_NUM_4, 0); // press (falling edge)
        drv.setLevel(GPIO_NUM_4, 1); // release
    }
}

void loop() {
    int count = buttonPresses.load();
    printf("[gpio_interrupt] Button pressed %d times\n", count);

    // Toggle LED based on odd/even
    gpio_set_level(GPIO_NUM_2, count % 2);
    printf("[gpio_interrupt] LED: %s\n", (count % 2) ? "ON" : "OFF");
}
